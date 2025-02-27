#pragma  once

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "fs_param.h"


using std::string;
using std::vector;

// list of directories
using Path = vector<string>;

inline string path_to_string(Path path) {
    string out = "";
    for (string node : path) {
        out += "/" + node;
    }
    return out;
}

struct FS_READBLOCK {
    string username;
    Path path;
    uint32_t block;
    bool operator==(const FS_READBLOCK&) const = default;
};

struct FS_WRITEBLOCK {
    string username;
    Path path;
    uint32_t block;
    std::shared_ptr<std::array<char, FS_BLOCKSIZE>> data;
    bool operator==(const FS_WRITEBLOCK&) const = default;
};

struct FS_CREATE {
    string username;
    Path path;
    // could be changed to a typesafe enum
    char type;
    bool operator==(const FS_CREATE&) const = default;
};

struct FS_DELETE {
    string username;
    Path path;
    bool operator==(const FS_DELETE&) const = default;
};

using Request = std::variant<FS_READBLOCK, FS_WRITEBLOCK, FS_CREATE, FS_DELETE>;

// a ton of fun shenanigans to make printing nice
inline string request_string(const FS_READBLOCK& r) {
    return "FS_READBLOCK " + r.username + " " + path_to_string(r.path) + " "
         + std::to_string(r.block);
}

inline string request_string(const FS_WRITEBLOCK& r) {
    return "FS_WRITEBLOCK " + r.username + " " + path_to_string(r.path) + " "
         + std::to_string(r.block);
}

inline string request_string(const FS_CREATE& r) {
    return "FS_CREATE " + r.username + " " + path_to_string(r.path) + " "
         + r.type;
}

inline string request_string(const FS_DELETE& r) {
    return "FS_DELETE " + r.username + " " + path_to_string(r.path);
}

// inline string request_string(std::monostate const&) {
//     return "{}";
// }

inline string request_string(Request const& r) {
    return std::visit([](auto&& arg) { return request_string(arg); }, r);
}

inline std::ostream& operator<<(std::ostream& os, Request const& v) {
    os << request_string(v);
    // std::visit([&os](auto&& arg) { os << request_string(arg); }, v);
    return os;
}

// tehre are fancier kinds of templates that can make above nicer
