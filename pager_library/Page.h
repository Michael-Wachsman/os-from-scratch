#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "vm_arena.h"
#include "vm_pager.h"

const uint64_t VM_ARENA_MAXADDR = reinterpret_cast<uint64_t>(VM_ARENA_BASEADDR) + VM_ARENA_SIZE;
const uint64_t NUM_VAs = VM_ARENA_SIZE / VM_PAGESIZE;

struct PhysicalPage {
    uint64_t PPN;
    bool clocked = false;
    bool dirty = false;
};

struct VirtualPage {
    std::vector<page_table_entry_t*> affiliated;
    bool isSwap;
    std::string filename = "";
    std::vector<unsigned int> blocks;   // if multiple swap blocks, writing to first one
    std::unique_ptr<PhysicalPage> physicalPage = nullptr;
    bool resident = false;

    // update all entries with new correct bits
    void updatePTEs();
    // void setAllRead(bool val);
    // void setAllWrite(bool val);
};
