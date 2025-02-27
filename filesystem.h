
#include <array>
#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#include <boost/thread/pthread/mutex.hpp>
#include <boost/thread/pthread/shared_mutex.hpp>

#include "fs_param.h"
#include "fs_server.h"
#include "request.h"

using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::weak_ptr;
using std::optional;
using out_block = std::shared_ptr<std::array<char, FS_BLOCKSIZE>>;

template <typename T>
struct inode_lock_info {
    fs_inode inode;
    shared_ptr<boost::shared_mutex> lock_ptr;
    T lock; // destructor is called first
    uint32_t block;
};

void fs_init();

// returns a block if read request, nullptr if success, {} if error
optional<out_block> execute_request(Request const& req);

optional<out_block> execute_request(FS_CREATE const& req);
optional<out_block> execute_request(FS_DELETE const& req);
optional<out_block> execute_request(FS_READBLOCK const& req);
optional<out_block> execute_request(FS_WRITEBLOCK const& req);

//might need to be defined fully in header I forget
template <typename T>
optional<inode_lock_info<T>> traverse_fs(Path const& path, string const& owner);

shared_ptr<boost::shared_mutex> get_lock_ptr(int block);

void check_disk_blocks_synced();
