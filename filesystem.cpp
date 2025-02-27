#include "filesystem.h"

#include <array>
#include <cstdint>
#include <deque>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <boost/thread/lock_types.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <boost/thread/pthread/shared_mutex.hpp>

#include "fs_param.h"
#include "fs_server.h"

boost::mutex internal_lock_free_list;
std::deque<uint32_t> free_disk_blocks;

constexpr bool strong_asserts = false;

// just for testing, will break in multithreading
// shared_ptr<boost::shared_mutex> keep_alive;

// spec says not to use reader/writer, but I think this makes sense for allocation
// boost::shared_mutex internal_rw_lock;
// block(equivalent to path)-> lock
boost::mutex internal_lock_map;
unordered_map<uint32_t, weak_ptr<boost::shared_mutex>> data_locks;

void initial_traverse_helper(unordered_set<uint32_t>& blocks_used,
                             uint32_t current_inode_block) {
    // hopefully no block too big issues
    fs_inode current_inode;
    disk_readblock(current_inode_block, &current_inode);

    for (uint32_t i = 0; i < current_inode.size; ++i) {
        auto block = current_inode.blocks[i];
        // we should not be traversing same spot multiple times
        assert(!blocks_used.contains(block));

        blocks_used.insert(block);
    }

    switch (current_inode.type) {
        case 'f':
            // should be done here
            break;
        case 'd':
            // need to recurse on dir entries
            for (uint32_t i = 0; i < current_inode.size; ++i) {
                auto block = current_inode.blocks[i];
                std::array<fs_direntry, FS_DIRENTRIES> direntries;
                disk_readblock(block, direntries.data());
                // need to deal with empty entries
                for (unsigned int i = 0; i < FS_DIRENTRIES; ++i) {
                    fs_direntry& entry = direntries[i];
                    if (entry.inode_block == 0) {
                        // I think correct should be left shifted, but it may not be
                        continue;
                    }
                    assert(!blocks_used.contains(entry.inode_block));
                    blocks_used.insert(entry.inode_block);
                    initial_traverse_helper(blocks_used, entry.inode_block);
                }
            }
            break;
        default:
            // should be unreachable
            assert(false);
    }
}

void fs_init() {
    // root stored at block 0
    // do an initial traversal to determine what is used
    std::unordered_set<uint32_t> blocks_used;
    // traverse and grab all used blocks
    initial_traverse_helper(blocks_used, 0);

    for (uint32_t i = 1; i < FS_DISKSIZE; i++) {
        if (!blocks_used.contains(i)) {
            free_disk_blocks.push_back(i);
        }
    }
}

// returns a block if read request
optional<out_block> execute_request(Request const& req) {
    return std::visit([](auto& arg) { return execute_request(arg); }, req);
}

optional<out_block> execute_request(FS_CREATE const& req) {
    printf("Req path size %zu\n", req.path.size());
    // possible errors - file already exists, bad path
    // no space left on disk or directory
    if (req.path.empty()) {
        return {};
    }

    {
        boost::unique_lock<boost::mutex> guard(internal_lock_free_list);
        if (free_disk_blocks.empty()) {
            return {};
        }
    }

    vector<string> target_path = req.path;
    target_path.pop_back();
    // we want an upgradeable lock in case we need to write to parent
    auto dir_inode_lock = traverse_fs<boost::upgrade_lock<boost::shared_mutex>>(
      target_path, req.username);
    printf("GOT PAST THE RETURN IN CREATE\n");

    if (!dir_inode_lock.has_value()) {
        return {};
    }

    if (dir_inode_lock->inode.type == 'f') {
        return {};
    }

    auto& dir_inode = dir_inode_lock->inode;
    auto filename = req.path.back();

    // could be a single type
    int next_free_spot_block = -1;
    int next_free_spot_arr_index = -1;
    std::array<fs_direntry, FS_DIRENTRIES> next_free_spot_direntries;

    for (uint32_t i = 0; i < dir_inode.size; ++i) {
        auto dir_array_block = dir_inode.blocks[i];

        std::array<fs_direntry, FS_DIRENTRIES> direntries;

        disk_readblock(dir_array_block, direntries.data());
        for (uint32_t j = 0; j < FS_DIRENTRIES; ++j) {
            fs_direntry& entry = direntries[j];
            // file already exists
            if (entry.inode_block != 0
                && strcmp(entry.name, filename.c_str()) == 0) {
                return {};
            }

            // if we found a first free spot
            if (entry.inode_block == 0 && next_free_spot_block == -1) {
                next_free_spot_arr_index = j;
                next_free_spot_block = dir_array_block;
                next_free_spot_direntries = direntries;
            }
        }
    }

    int new_inode_block;
    {
        boost::unique_lock<boost::mutex> guard(internal_lock_free_list);
        // allocate new inode first
        if (free_disk_blocks.empty()) {
            return {};
        }
        new_inode_block = free_disk_blocks.front();
        free_disk_blocks.pop_front();
    }

    fs_inode new_inode;
    new_inode.size = 0;
    new_inode.type = req.type;
    strcpy(new_inode.owner, req.username.c_str());

    // couldn't find a free spot,
    if (next_free_spot_block == -1) {
        // and we're out of space
        if (dir_inode.size == FS_MAXFILEBLOCKS) {
            {
                boost::unique_lock guard(internal_lock_free_list);
                free_disk_blocks.push_front(new_inode_block);
            }
            return {};
        }
        else {
            // make a new direntries block
            uint32_t new_direntries_block;
            {
                boost::unique_lock guard(internal_lock_free_list);
                if (free_disk_blocks.empty()) {
                    // give the old one back since we won't use it
                    free_disk_blocks.push_front(new_inode_block);
                    return {};
                }
                new_direntries_block = free_disk_blocks.front();
                free_disk_blocks.pop_front();
            }

            std::array<fs_direntry, FS_DIRENTRIES> new_direntries {};
            new_direntries[0].inode_block = new_inode_block;
            strcpy(new_direntries[0].name, filename.c_str());

            // all data setup, write new blocks in-- double check ordering in spec
            disk_writeblock(new_direntries_block, new_direntries.data());
            disk_writeblock(new_inode_block, &new_inode);

            // now finally acquire write lock and update top dir
            dir_inode.blocks[dir_inode.size] = new_direntries_block;
            dir_inode.size += 1;
            boost::unique_lock writer_lock(std::move(dir_inode_lock->lock));
            disk_writeblock(dir_inode_lock->block, &dir_inode);

            return nullptr;
        }
    }

    // we did find an opening, update structures
    next_free_spot_direntries[next_free_spot_arr_index].inode_block
      = new_inode_block;
    strcpy(next_free_spot_direntries[next_free_spot_arr_index].name,
           filename.c_str());

    // write new inode
    disk_writeblock(new_inode_block, &new_inode);

    // acquire writelock for top
    boost::unique_lock writer_lock(std::move(dir_inode_lock->lock));
    disk_writeblock(next_free_spot_block, &next_free_spot_direntries);

    if (strong_asserts) {
        check_disk_blocks_synced();
    }

    return nullptr;
}

optional<out_block> execute_request(FS_DELETE const& req) {
    // possible errors - file/path doesn't exist, file is not accessible by this user
    // a directory with no files or subdirectories
    // root directory cannot be deleted
    if (req.path.empty()) {
        return {};
    }

    {
        boost::unique_lock<boost::mutex> guard(internal_lock_free_list);
        if (free_disk_blocks.size() == 4095) {
            return {};
        }
    }

    auto parent_path = req.path;
    parent_path.pop_back();

    auto parent_inode_info = traverse_fs<boost::upgrade_lock<boost::shared_mutex>>(
      parent_path, req.username);

    if (!parent_inode_info.has_value()) {
        return {};
    }

    auto& parent_inode = parent_inode_info->inode;

    if (parent_inode.size == 0) {
        return {};
    }

    // parent needs to be a directory for us to inspect properly
    if (parent_inode.type == 'f') {
        return {};
    }

    uint32_t target_block;
    fs_inode target_inode;

    size_t direntries_index_in_parent = -1;
    size_t found_spot_direntries_block = -1;
    size_t found_spot_direntries_index = -1;
    std::array<fs_direntry, FS_DIRENTRIES> found_spot_direntries;

    shared_ptr<boost::shared_mutex> target_lock_ptr;
    boost::upgrade_lock<boost::shared_mutex> target_lock;
    boost::unique_lock<boost::shared_mutex> parent_writer_guard;

    std::array<fs_direntry, FS_DIRENTRIES> direntries;
    bool found_target = false;
    for (size_t i = 0; i < parent_inode.size; i++) {
        disk_readblock(parent_inode.blocks[i], direntries.data());
        for (size_t j = 0; j < direntries.size(); j++) {
            if (direntries[j].inode_block != 0
                && strcmp(direntries[j].name, req.path.back().c_str()) == 0) {
                {
                    target_lock_ptr = get_lock_ptr(direntries[j].inode_block);

                    // get writer lock for parent
                    boost::unique_lock<boost::shared_mutex> new_writer_guard(
                      std::move(parent_inode_info->lock));
                    new_writer_guard.swap(parent_writer_guard);

                    boost::upgrade_lock<boost::shared_mutex> tmp(*target_lock_ptr);
                    target_lock.swap(tmp);
                }

                disk_readblock(direntries[j].inode_block, &target_inode);
                target_block = direntries[j].inode_block;

                direntries_index_in_parent = i;
                found_spot_direntries_block = parent_inode.blocks[i];
                found_spot_direntries_index = j;
                found_spot_direntries = direntries;

                found_target = true;
                break;
            }
        }
        if (found_target) {
            break;
        }
    }
    // for deletion ordering reasons, target lock must get released
    if (!found_target) {
        // no target lock held, we are good here
        return {};
    }

    // verify we own this final node
    if (strcmp(target_inode.owner, req.username.c_str()) != 0) {
        target_lock.unlock();
        target_lock.release();
        return {};
    }

    // verify empty directory
    if (target_inode.type == 'd' && target_inode.size > 0) {
        target_lock.unlock();
        target_lock.release();
        return {};
    }

    // check if anything else in the affiliated direntries
    bool can_delete_direntries = true;
    for (auto& entry : found_spot_direntries) {
        if (entry.inode_block != 0 && entry.inode_block != target_block) {
            can_delete_direntries = false;
            break;
        }
    }

    // We don't need child to be a writer yet
    if (can_delete_direntries) {
        // need to shift over one at a time
        for (size_t i = direntries_index_in_parent; i < parent_inode.size - 1; ++i) {
            parent_inode.blocks[i] = parent_inode.blocks[i + 1];
        }
        parent_inode.size -= 1;

        disk_writeblock(parent_inode_info->block, &parent_inode);

        // give back the direntries block
        {
            boost::unique_lock guard(internal_lock_free_list);
            free_disk_blocks.push_back(found_spot_direntries_block);
        }
    }
    else {
        found_spot_direntries[found_spot_direntries_index].inode_block = 0;
        disk_writeblock(found_spot_direntries_block, &found_spot_direntries);
    }

    // we now can drop parent lock, now that file system pointer has been changed
    // no one will go after child
    parent_writer_guard.unlock();

    // clear all readers off of child
    boost::unique_lock target_writer_lock(std::move(target_lock));
    // don't let anyone else ever get this lock
    {
        boost::unique_lock<boost::mutex> guard(internal_lock_map);
        data_locks.erase(target_block);
    }

    // can technically drop writer if we want now, no one should be waiting tho
    {
        boost::unique_lock guard(internal_lock_free_list);
        // if it's a file, give back all data blocks to free list
        if (target_inode.type == 'f') {
            for (size_t i = 0; i < target_inode.size; ++i) {
                free_disk_blocks.push_back(target_inode.blocks[i]);
            }
        }

        // give back the inode itself
        free_disk_blocks.push_back(target_block);
    }

    if (strong_asserts) {
        check_disk_blocks_synced();
    }
    return nullptr;
}

optional<out_block> execute_request(FS_READBLOCK const& req) {
    // possible errors - file/path doesn't exist, file is not accessible by this user

    auto out = std::make_shared<std::array<char, FS_BLOCKSIZE>>();

    if (req.path.empty()) {
        return {};
    }

    if (req.block >= FS_MAXFILEBLOCKS) {
        return {};
    }

    auto dir_inode_lock
      = traverse_fs<boost::shared_lock<boost::shared_mutex>>(req.path, req.username);
    if (!dir_inode_lock.has_value()) {
        return {};
    }

    if (dir_inode_lock->inode.type == 'd') {
        return {};
    }

    if (dir_inode_lock->inode.size <= req.block) {
        return {};
    }

    printf("Reading block %u\n", req.block);

    disk_readblock(dir_inode_lock->inode.blocks[req.block], out->data());

    if (strong_asserts) {
        check_disk_blocks_synced();
    }
    return out;
}

optional<out_block> execute_request(FS_WRITEBLOCK const& req) {
    // possible errors - file/path doesn't exist, file is not accessible by this user
    // check space on disk and in file(could alloc I think)
    // can only write to existing blocks or block immediately after current EOF
    if (req.path.empty()) {
        return {};
    }

    // can't ever write to a block over this size
    if (req.block >= FS_MAXFILEBLOCKS) {
        return {};
    }

    auto file_inode_info = traverse_fs<boost::upgrade_lock<boost::shared_mutex>>(
      req.path, req.username);
    // Path doesn't exist or is inaccessible by user
    if (!file_inode_info.has_value()) {
        return {};
    }
    // Path is a directory
    if (file_inode_info->inode.type == 'd') {
        return {};
    }

    auto& file_inode = file_inode_info->inode;
    // Check if block exists
    if (req.block < file_inode.size) {
        boost::unique_lock writer_lock(std::move(file_inode_info->lock));
        disk_writeblock(file_inode.blocks[req.block], req.data->data());
        return nullptr;
    }
    // Check if right past EOF (last block + 1)
    if (req.block == file_inode.size) {
        // Allocate the new block and update the inode
        {
            boost::unique_lock guard(internal_lock_free_list);
            if (free_disk_blocks.empty()) {
                return {};
            }
            file_inode.blocks[file_inode.size] = free_disk_blocks.front();
            free_disk_blocks.pop_front();
        }
        file_inode.size++;
        disk_writeblock(file_inode.blocks[req.block], req.data->data());
        boost::unique_lock writer_lock(std::move(file_inode_info->lock));
        disk_writeblock(file_inode_info->block, &file_inode);

        if (strong_asserts) {
            check_disk_blocks_synced();
        }
        return nullptr;
    }
    return {};
}

template <typename T>
optional<inode_lock_info<T>> traverse_fs(Path const& path, string const& owner) {
    // makes no sense to traverse an empty path, should be filtered out on other side
    printf("PATH SIZE: %zu\n", path.size());

    // only the final node should be locked with T name lock, the rest can be reads
    fs_inode current_inode;
    uint32_t current_inode_block;

    shared_ptr<boost::shared_mutex> current_lock_ptr = get_lock_ptr(0);
    shared_ptr<boost::shared_mutex> next_lock_ptr;

    boost::shared_lock<boost::shared_mutex> current_lock;
    T final_lock;
    // to acquire needs to live and be in sync with the currently held lock
    // if path is empty, return final lock to the root and its data
    if (path.size() == 0) {
        T root_lock(*current_lock_ptr);
        final_lock.swap(root_lock);
        disk_readblock(0, &current_inode);
        return inode_lock_info<T> { current_inode, current_lock_ptr,
                                    std::move(final_lock), 0 };
    }
    else {
        boost::shared_lock root_lock(*current_lock_ptr);
        current_lock.swap(root_lock);
    }

    // Start at the root inode
    disk_readblock(0, &current_inode);

    for (size_t i = 0; i < path.size(); ++i) {
        // Should not find a file in the middle of the path
        if (current_inode.type == 'f') {
            printf("failing because not a file\n");
            return {};
        }
        // Everyone can access root
        if (strcmp(owner.c_str(), current_inode.owner) != 0 && i != 0) {
            printf("failing bc of owner\n");
            return {};
        }

        bool inode_found = false;
        for (uint32_t block_idx = 0; block_idx < current_inode.size; ++block_idx) {
            auto block = current_inode.blocks[block_idx];
            std::array<fs_direntry, FS_DIRENTRIES> direntries;

            disk_readblock(block, direntries.data());
            for (unsigned int j = 0; j < FS_DIRENTRIES; ++j) {
                fs_direntry entry = direntries[j];
                if (entry.inode_block != 0
                    && strcmp(path[i].c_str(), entry.name) == 0) {
                    next_lock_ptr = get_lock_ptr(entry.inode_block);

                    if (i == path.size() - 1) {
                        T swap_lock(*next_lock_ptr);
                        final_lock.swap(swap_lock);

                        // so it doesn't hold onto any old locks
                        current_lock.unlock();
                        current_lock.release();
                    }
                    else {
                        boost::shared_lock swap_lock(*next_lock_ptr);
                        current_lock.swap(swap_lock);
                    }
                    current_lock_ptr.swap(next_lock_ptr);

                    current_inode_block = entry.inode_block;
                    disk_readblock(entry.inode_block, &current_inode);
                    inode_found = true;

                    break;
                }
            }
            if (inode_found) {
                break;
            }
        }
        // No match for the current path[i] name
        if (!inode_found) {
            printf("could not find path\n");
            return {};
        }
    }

    if (strcmp(owner.c_str(), current_inode.owner) != 0) {
        printf("end owner issue\n");
        return {};
    }

    // current lock ptr needs to live with everything else
    return inode_lock_info<T> {
        current_inode,
        current_lock_ptr,
        std::move(final_lock),
        current_inode_block,
    };
}

// only code that gets lock on lock_map
shared_ptr<boost::shared_mutex> get_lock_ptr(int block) {
    shared_ptr<boost::shared_mutex> out;
    boost::unique_lock guard(internal_lock_map);
    auto weak_to_acquire = data_locks[block];

    // can't do a separate expired check with thread safety
    out = data_locks[block].lock();

    if (out == nullptr) {
        printf("MADE A LOCK\n");
        // need to allocate lock, change the weak ptr in data_locks
        out = std::make_shared<boost::shared_mutex>();
        data_locks[block] = out;
    }

    return out;
}

void check_disk_blocks_synced() {
    boost::unique_lock<boost::mutex> guard(internal_lock_free_list);
    std::set<unsigned int> old_free_list(free_disk_blocks.begin(),
                                         free_disk_blocks.end());
    fs_init();
    std::set<unsigned int> new_free_list(free_disk_blocks.begin(),
                                         free_disk_blocks.end());
    assert(old_free_list == new_free_list);
}
