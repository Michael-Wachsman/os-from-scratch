#include "vm_pager.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Page.h"
#include "vm_arena.h"

using std::array;
using std::cout;
using std::deque;
using std::endl;
using std::optional;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

constexpr bool strong_asserts = true;

constexpr uint page_table_size = VM_ARENA_SIZE / VM_PAGESIZE;
unordered_map<pid_t, unique_ptr<array<page_table_entry_t, page_table_size>>>
  process_table;
unordered_map<pid_t, unordered_map<uint64_t, shared_ptr<VirtualPage>>> translateVA;
unordered_map<pid_t, uint64_t> next_free_VA;
// Need to have both filename and block as keys
unordered_map<string, unordered_map<unsigned int, shared_ptr<VirtualPage>>>
  filename_block_to_vpage;

// don't need to be FIFO, but doing so for simplicity
unsigned int max_swap_blocks;
deque<unsigned int> next_free_swap;

deque<unsigned int> next_free_PPN;

deque<std::weak_ptr<VirtualPage>> clock_queue;
unsigned int mem_pages_cap;
pid_t current_pid;

void evict_and_replace(shared_ptr<VirtualPage> replacement);

void assert_structure_consistency();

page_table_entry_t* VA_to_PTE(uint64_t va, page_table_entry_t* ptbr) {
    return ptbr
         + ((va - reinterpret_cast<uint64_t>(VM_ARENA_BASEADDR)) / VM_PAGESIZE);
}

void give_new_ppn(shared_ptr<VirtualPage> vp) {
    // assert(!vp->resident || vp->physicalPage->PPN == 0);
    vp->physicalPage = std::make_unique<PhysicalPage>();
    if (!next_free_PPN.empty()) {
        // find a new PPN
        vp->physicalPage->PPN = next_free_PPN.front();
        next_free_PPN.pop_front();
        vp->physicalPage->clocked = true;
        clock_queue.push_back(vp);
    } else {
        evict_and_replace(vp);
    }
    vp->resident = true;
}

void return_ppn_remove_from_clock(shared_ptr<VirtualPage> vp) {
    auto same_as_weak
      = [&vp](std::weak_ptr<VirtualPage> const& a) { return a.lock() == vp; };
    next_free_PPN.push_back(vp->physicalPage->PPN);
    auto our_clocker
      = std::find_if(clock_queue.begin(), clock_queue.end(), same_as_weak);
    // assert(our_clocker != clock_queue.end());
    clock_queue.erase(our_clocker);
    vp->resident = false;
    vp->physicalPage = nullptr;
    vp->updatePTEs();
}

shared_ptr<VirtualPage> make_swap_to_nullpage(uint64_t virtual_addr, pid_t pid) {
    // swap backed page
    if (next_free_swap.size() == 0) {
        if (strong_asserts) {
            assert_structure_consistency();
        }
        return nullptr;
    }

    translateVA[pid][virtual_addr] = std::make_shared<VirtualPage>();
    auto vp = translateVA[pid][virtual_addr];
    vp->blocks.push_back(next_free_swap.front());
    next_free_swap.pop_front();
    vp->resident = true;
    vp->physicalPage = std::make_unique<PhysicalPage>();
    vp->physicalPage->PPN = 0;
    vp->physicalPage->dirty = false;
    vp->physicalPage->clocked = true;
    vp->isSwap = true;
    vp->affiliated.push_back(VA_to_PTE(virtual_addr, process_table[pid]->data()));
    vp->updatePTEs();
    return vp;
}

void remove_from_filename_block(shared_ptr<VirtualPage> vp) {
    // assert(filename_block_to_vpage.contains(vp->filename)
    // and filename_block_to_vpage[vp->filename].contains(vp->blocks.front()));
    filename_block_to_vpage[vp->filename].erase(vp->blocks.front());

    if (filename_block_to_vpage[vp->filename].size() == 0) {
        filename_block_to_vpage.erase(vp->filename);
    }
}

std::optional<string> translate_from_other_addr_space(char const* their_filename,
                                                      pid_t process) {
    auto va_filename = reinterpret_cast<uint64_t>(their_filename);
    auto va_filename_block = va_filename - (va_filename % VM_PAGESIZE);
    if (!translateVA[current_pid].contains(va_filename_block)) {
        return {};
    }

    // assert(!translateVA[current_pid][va_filename_block]->affiliated.empty());
    if (!translateVA[current_pid][va_filename_block]
           ->affiliated.front()
           ->read_enable) {
        int res = vm_fault(reinterpret_cast<void*>(va_filename), false);
        if (res == -1) {
            return {};
        }
    }
    // assert(translateVA[current_pid][va_filename_block]->resident);

    char* our_filename_start
      = reinterpret_cast<char*>(vm_physmem)
      + (translateVA[current_pid][va_filename_block]->physicalPage->PPN
         * VM_PAGESIZE)
      + (va_filename % VM_PAGESIZE);

    // WE BE THE MMU and strcpy
    string running = "";
    while (*our_filename_start) {
        running += *our_filename_start;

        // check that are not crossing
        our_filename_start += 1;
        va_filename += 1;

        // have to load a new block
        if (va_filename >= va_filename_block + VM_PAGESIZE) {
            auto va_filename_block = va_filename - (va_filename % VM_PAGESIZE);

            if (!translateVA[current_pid].contains(va_filename_block)) {
                return {};
            }

            // if not readable throw a fault
            // assert(!translateVA[current_pid][va_filename_block]->affiliated.empty());
            if (!translateVA[current_pid][va_filename_block]
                   ->affiliated.front()
                   ->read_enable) {
                int res = vm_fault(reinterpret_cast<void*>(va_filename), false);
                if (res == -1) {
                    return {};
                }
            }

            // recalculate where it is in phys mem
            our_filename_start
              = reinterpret_cast<char*>(vm_physmem)
              + (translateVA[current_pid][va_filename_block]->physicalPage->PPN
                 * VM_PAGESIZE)
              + (va_filename % VM_PAGESIZE);
        }
    }
    return running;
}

void vm_init(unsigned int memory_pages, unsigned int swap_blocks) {
    std::memset(vm_physmem, 0, VM_PAGESIZE);
    mem_pages_cap = memory_pages;
    max_swap_blocks = swap_blocks;

    for (unsigned int i = 1; i < memory_pages; i++) {
        next_free_PPN.push_back(i);
    }

    for (unsigned int i = 0; i < max_swap_blocks; i++) {
        next_free_swap.push_back(i);
    }
}

int vm_create(pid_t parent_pid, pid_t child_pid) {
    auto table = std::make_unique<array<page_table_entry_t, page_table_size>>();
    process_table.insert({ child_pid, std::move(table) });

    next_free_VA[child_pid] = reinterpret_cast<uint64_t>(VM_ARENA_BASEADDR);
    if (translateVA.contains(parent_pid)) {
        // count how many swap we will need
        uint64_t swap_needed = 0;
        for (auto [va, vpage] : translateVA[parent_pid]) {
            if (vpage->isSwap) {
                swap_needed += 1;
            }
        }

        if (next_free_swap.size() < swap_needed) {
            // get rid of the other structures
            process_table.erase(child_pid);
            next_free_VA.erase(child_pid);
            return -1;
        }
        next_free_VA[child_pid] = next_free_VA[parent_pid];

        // add initial VAs and add to affiliated
        for (auto [va, vpage] : translateVA[parent_pid]) {
            // If we point to the null page, just make a new structure,
            // simplifying things
            if (vpage->resident && vpage->physicalPage->PPN == 0) {
                auto res = make_swap_to_nullpage(va, child_pid);
                // assert(res != nullptr);
                continue;
            }

            translateVA[child_pid][va] = vpage;

            if (vpage->isSwap) {
                vpage->blocks.push_back(next_free_swap.front());
                next_free_swap.pop_front();
            }

            // this needs to be after, in case of swap fault issues
            vpage->affiliated.push_back(
              VA_to_PTE(va, process_table[child_pid]->data()));

            vpage->updatePTEs();
        }
    }
    // only runs first time
    if (!process_table.contains(parent_pid)) {
        current_pid = child_pid;
        page_table_base_register = process_table[current_pid]->data();
    }
    if (strong_asserts) {
        assert_structure_consistency();
    }
    return 0;
}

void* vm_map(char const* their_filename, unsigned int block) {
    auto virtual_addr = next_free_VA[current_pid];

    // Check if next available virtual address goes out of bounds
    if (virtual_addr >= VM_ARENA_MAXADDR) {
        if (strong_asserts) {
            assert_structure_consistency();
        }
        return nullptr;
    }

    if (their_filename) {
        string our_filename;
        std::optional<string> try_convert
          = translate_from_other_addr_space(their_filename, current_pid);
        if (try_convert.has_value()) {
            our_filename = try_convert.value();
        } else {
            // couldn't translate properly
            return nullptr;
        }

        // file backed page
        if (filename_block_to_vpage.contains(our_filename)
            && filename_block_to_vpage[our_filename].contains(block)) {
            translateVA[current_pid][virtual_addr] = shared_ptr<VirtualPage>(
              filename_block_to_vpage[our_filename][block]);
            auto vp = translateVA[current_pid][virtual_addr];

            vp->affiliated.push_back(
              VA_to_PTE(virtual_addr, page_table_base_register));
            vp->updatePTEs();
        } else {
            // need to create new filebacked page
            auto vp = std::make_shared<VirtualPage>();
            filename_block_to_vpage[our_filename][block] = vp;
            translateVA[current_pid][virtual_addr] = vp;
            vp->resident = false;
            vp->filename = our_filename;
            vp->isSwap = false;
            vp->blocks.push_back(block);
            vp->affiliated.push_back(
              VA_to_PTE(virtual_addr, page_table_base_register));

            vp->updatePTEs();
        }
    } else {
        auto res = make_swap_to_nullpage(virtual_addr, current_pid);
        if (res == nullptr) {
            return nullptr;
        }
    }

    next_free_VA[current_pid] += VM_PAGESIZE;
    if (strong_asserts) {
        assert_structure_consistency();
    }
    return reinterpret_cast<void*>(virtual_addr);
}

void vm_switch(pid_t pid) {
    current_pid = pid;
    // assert(process_table.contains(current_pid));
    page_table_base_register = process_table[current_pid]->data();
    if (strong_asserts) {
        assert_structure_consistency();
    }
}

int vm_fault(void const* addr, bool write_flag) {
    uint64_t va_block_addr = reinterpret_cast<uint64_t>(addr)
                           - reinterpret_cast<uint64_t>(addr) % VM_PAGESIZE;
    if (!translateVA[current_pid].contains(va_block_addr)) {
        if (strong_asserts) {
            assert_structure_consistency();
        }
        return -1;
    }
    auto fault_page = translateVA[current_pid][va_block_addr];

    // copy-on-write
    if (write_flag && fault_page->blocks.size() > 1) {
        // assert(fault_page->isSwap);

        // do a read on fault page
        if (!fault_page->affiliated.front()->read_enable) {
            int res = vm_fault(addr, false);
            if (res == -1) {
                return -1;
            }
        }

        // move respective affiliates from original to copy
        auto new_vpage = std::make_shared<VirtualPage>();
        new_vpage->affiliated.push_back(
          VA_to_PTE(va_block_addr, page_table_base_register));

        translateVA[current_pid][va_block_addr] = new_vpage;

        auto pte_in_fault
          = std::find(fault_page->affiliated.begin(), fault_page->affiliated.end(),
                      VA_to_PTE(va_block_addr, page_table_base_register));
        // assert(pte_in_fault != fault_page->affiliated.end());
        fault_page->affiliated.erase(pte_in_fault);

        new_vpage->blocks.push_back(fault_page->blocks.back());
        fault_page->blocks.pop_back();

        // assert(!fault_page->blocks.empty());
        // assert(fault_page->resident);

        new_vpage->isSwap = true;
        give_new_ppn(new_vpage);
        // assert(new_vpage->resident);


        // copy the data over, if we didn't happen to just evict the last
        // page
        if (fault_page->resident) {
            memcpy(reinterpret_cast<char*>(vm_physmem)
                     + (new_vpage->physicalPage->PPN * VM_PAGESIZE),
                   reinterpret_cast<char*>(vm_physmem)
                     + (fault_page->physicalPage->PPN * VM_PAGESIZE),
                   VM_PAGESIZE);
        }
        // if we happened to just evict it, then the data is already
        // correct!

        new_vpage->physicalPage->dirty = true;
        new_vpage->physicalPage->clocked = true;
        fault_page->updatePTEs();
        new_vpage->updatePTEs();
        if (strong_asserts) {
            assert_structure_consistency();
        }
        return 0;
    }

    // we must allocate a new page
    if (!fault_page->resident || fault_page->physicalPage->PPN == 0) {
        fault_page->physicalPage = std::make_unique<PhysicalPage>();
        bool isNullPage = fault_page->resident;

        give_new_ppn(fault_page);

        if (isNullPage) {
            // copy zero page into other part of mem
            memset(reinterpret_cast<char*>(vm_physmem)
                     + (fault_page->physicalPage->PPN * VM_PAGESIZE),
                   0, VM_PAGESIZE);
        } else {
            int res = file_read(
              fault_page->isSwap ? nullptr : fault_page->filename.c_str(),
              fault_page->blocks.front(),
              reinterpret_cast<void*>(
                reinterpret_cast<char*>(vm_physmem)
                + (fault_page->physicalPage->PPN * VM_PAGESIZE)));

            if (res == -1) {
                // vm_destroy does not get rid of filebacked, so we must
                if (fault_page->affiliated.size() == 1) {
                    remove_from_filename_block(fault_page);
                }

                return_ppn_remove_from_clock(fault_page);

                if (strong_asserts) {
                    assert_structure_consistency();
                }
                return -1;
            }
        }
        fault_page->resident = true;
    }

    if (!fault_page->physicalPage->dirty) {
        fault_page->physicalPage->dirty = write_flag;
    } else if (write_flag) {
        // should only be incurring a write fault on a dirty page if it's cow
        // or if was not clocked
        assert(fault_page->blocks.size() > 1 || !fault_page->physicalPage->clocked);
    }
    fault_page->physicalPage->clocked = true;

    fault_page->updatePTEs();
    if (strong_asserts) {
        assert_structure_consistency();
    }
    return 0;
}

void vm_destroy() {
    // assert(process_table.contains(current_pid));

    for (auto [va, vpage] : translateVA[current_pid]) {
        // assert(vpage->affiliated.size() != 0);

        if (vpage->isSwap) {
            // if we are swap backed
            next_free_swap.push_back(vpage->blocks.back());
            vpage->blocks.pop_back();
        }

        if (vpage->resident && vpage->physicalPage->PPN != 0
            && vpage->blocks.size() == 0) {
            return_ppn_remove_from_clock(vpage);
        }

        // pop ourselves off affiliated
        auto our_aff = std::find(vpage->affiliated.begin(), vpage->affiliated.end(),
                                 VA_to_PTE(va, page_table_base_register));
        // assert(our_aff != vpage->affiliated.end());
        vpage->affiliated.erase(our_aff);
        if (!vpage->affiliated.empty()) {
            vpage->updatePTEs();
        }
    }


    translateVA.erase(current_pid);
    process_table.erase(current_pid);
    next_free_VA.erase(current_pid);
    if (strong_asserts) {
        assert_structure_consistency();
    }
    // cout << "Swap Size: " <<next_free_swap.size() << endl;
    // cout << "Open Mem Size: " <<next_free_PPN.size() << endl;
}

void evict_and_replace(shared_ptr<VirtualPage> replacement) {
    // shouldn't evict unless mem is full
    // assert(next_free_PPN.size() == 0);
    // assert(!clock_queue.empty());

    while (true) {
        // assert(!clock_queue.front().expired());
        auto front = clock_queue.front().lock();
        if (front->physicalPage->clocked) {
            front->physicalPage->clocked = false;

            // it's possible to have filebacked pages with no affiliates on clock
            // queue
            if (!front->affiliated.empty()) {
                front->updatePTEs();
            }
            clock_queue.pop_front();
            clock_queue.push_back(front);
        } else {
            break;
        }
    }

    auto to_evict = clock_queue.front().lock();
    clock_queue.pop_front();
    // assert(!to_evict->physicalPage->clocked);

    if (to_evict->physicalPage->dirty) {
        int res = file_write(to_evict->isSwap ? nullptr : to_evict->filename.c_str(),
                             to_evict->blocks.front(),
                             reinterpret_cast<char*>(vm_physmem)
                               + (to_evict->physicalPage->PPN * VM_PAGESIZE));

        // this should never happen if initial read in was ok
        // assert(res != -1);
    }
    // update replacement
    // assert(!replacement->resident || replacement->physicalPage->PPN == 0);

    auto same_as_replacement = [&replacement](std::weak_ptr<VirtualPage> const a) {
        return a.lock() == replacement;
    };

    // this assert is a bit slow, can remove if needed
    // assert(std::find_if(clock_queue.begin(), clock_queue.end(),
    // same_as_replacement)
    //        == clock_queue.end());

    replacement->physicalPage = std::make_unique<PhysicalPage>();
    replacement->physicalPage->PPN = to_evict->physicalPage->PPN;
    replacement->physicalPage->clocked = true;
    replacement->physicalPage->dirty = false;
    replacement->updatePTEs();
    clock_queue.push_back(replacement);

    //  set to non resident, update PTEs
    to_evict->resident = false;
    to_evict->physicalPage = nullptr;
    if (!to_evict->affiliated.empty()) {
        to_evict->updatePTEs();
    }

    // it's about to die, remove from filename_block
    if (!to_evict->isSwap && to_evict->affiliated.size() == 0) {
        remove_from_filename_block(to_evict);
    }
}


void assert_structure_consistency() {
    unordered_map<shared_ptr<VirtualPage>, size_t> num_mapped_to;

    // check all virtual pages
    for (auto [process_pid, process_vas] : translateVA) {
        for (auto [va, vpage] : process_vas) {
            if (!num_mapped_to.contains(vpage)) {
                num_mapped_to[vpage] = 1;
            } else {
                num_mapped_to[vpage] += 1;
            }

            assert(!vpage->blocks.empty());

            // make sure this VA not still available
            assert(next_free_VA.contains(process_pid));
            assert(va < next_free_VA[process_pid]);


            // manually check not dirty
            // this will display another file read
            if (vpage->resident && !vpage->isSwap && vpage->physicalPage->PPN != 0) {
                //  memcmp, using nullpage as temp(can't write anywhere
                //  outside of vm_physmem
                // int res = file_read(vpage->filename.c_str(), vpage->blocks.front(),
                //                     vm_physmem);
                // if (res == 0) {
                //     if (vpage->physicalPage->dirty) {
                //     } else {
                //         assert(!memcmp(vm_physmem,
                //                        reinterpret_cast<char*>(vm_physmem)
                //                          + vpage->physicalPage->PPN * VM_PAGESIZE,
                //                        VM_PAGESIZE));
                //     }
                // }

                // memset(vm_physmem, 0, VM_PAGESIZE);
            }

            if (vpage->isSwap) {
                // ensure that swap block is not marked as available
                for (auto swap_alloc : vpage->blocks) {
                    assert(std::find(next_free_swap.begin(), next_free_swap.end(),
                                     swap_alloc)
                           == next_free_swap.end());
                }

                assert(vpage->blocks.size() == vpage->affiliated.size());

                // if multiple affiliated, manually check write is false
                if (vpage->affiliated.size() > 1) {
                    for (auto aff : vpage->affiliated) {
                        assert(aff->write_enable == false);
                    }
                }

            } else {
                assert(vpage->blocks.size() == 1);

                // THESE ASSERTS FAIL FOR FILEBACKED PAGES WITH BAD FILES,
                // due to removal on bad file_read
                //  assert(filename_block_to_vpage.contains(vpage->filename));
                //  assert(filename_block_to_vpage[vpage->filename].contains(vpage->blocks.front()));
                //  assert(filename_block_to_vpage[vpage->filename][vpage->blocks.front()]
                //  == vpage);
            }

            // assert(vpage->resident != (vpage->physicalPage == nullptr));
            if (vpage->resident) {
                assert(vpage->physicalPage != nullptr);
            } else {
                assert(vpage->physicalPage == nullptr);
            }


            // ensure all PTEs are updated
            VirtualPage equiv_vpage_state;
            equiv_vpage_state.isSwap = vpage->isSwap;
            page_table_entry_t test_entry;
            for (size_t i = 0; i < vpage->affiliated.size(); ++i) {
                equiv_vpage_state.affiliated.push_back(&test_entry);
            }
            for (size_t i = 0; i < vpage->blocks.size(); ++i) {
                equiv_vpage_state.blocks.push_back(0);
            }
            equiv_vpage_state.resident = vpage->resident;
            equiv_vpage_state.filename
              = vpage->filename;   // since used for swap file indication
            if (vpage->resident) {
                equiv_vpage_state.physicalPage = std::make_unique<PhysicalPage>();
                equiv_vpage_state.physicalPage->dirty = vpage->physicalPage->dirty;
                equiv_vpage_state.physicalPage->clocked
                  = vpage->physicalPage->clocked;
                equiv_vpage_state.physicalPage->PPN = vpage->physicalPage->PPN;

                assert(std::find(next_free_PPN.begin(), next_free_PPN.end(),
                                 vpage->physicalPage->PPN)
                       == next_free_PPN.end());
            }
            equiv_vpage_state.updatePTEs();

            // verify that this va is attached to the affiliated
            auto intended_pte = VA_to_PTE(va, process_table[process_pid]->data());
            auto found_in_aff = std::find(vpage->affiliated.begin(),
                                          vpage->affiliated.end(), intended_pte);
            if (found_in_aff == vpage->affiliated.end()) {
                // cout << "Issue occurring with pid " << process_pid << endl;
            }
            assert(found_in_aff != vpage->affiliated.end());

            for (auto aff : vpage->affiliated) {
                // apparently equals isn't implemented?
                //
                assert(test_entry.read_enable == aff->read_enable);
                assert(test_entry.write_enable == aff->write_enable);
                if (vpage->resident) {
                    assert(test_entry.ppage == aff->ppage);
                }
            }

            // check that everything resident is in clock queue or null
            // page
            if (vpage->resident and vpage->physicalPage->PPN != 0) {
                // Message To Graders: For some reason vpage is not in scope if
                // it is captured by the lambda on my mac, but using gpage
                // apparently works
                auto gpage = vpage;
                auto is_vpage
                  = [gpage](auto other) { return other.lock() == gpage; };
                auto found
                  = std::find_if(clock_queue.begin(), clock_queue.end(), is_vpage);
                assert(found != clock_queue.end());
            }
        }
    }

    // verify the count in a second loop
    for (auto [pid, table] : translateVA) {
        for (auto [va, vpage] : table) {
            assert(vpage->affiliated.size() == num_mapped_to[vpage]);
        }
    }

    assert(clock_queue.size() == (mem_pages_cap - next_free_PPN.size() - 1));

    // check that nothing extraneous is in the clock_queue
    for (auto in_mem : clock_queue) {
        assert(!in_mem.expired());
        auto grabbed = in_mem.lock();
        assert(grabbed->resident);
        // we should not be evicting things that point to nullpage
        assert(grabbed->physicalPage->PPN != 0);
    }
}
