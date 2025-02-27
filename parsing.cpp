#include "parsing.h"

#include <cstdint>
#include <stdexcept>
#include <string>

#include "fs_param.h"


// note if final character is delimeter, will not create empty entry
vector<string> split(char const* msg, char delimeter) {
    vector<string> out;
    std::istringstream stream(msg);
    string s;

    // getline can read to a delimiter
    while (getline(stream, s, delimeter)) {
        out.push_back(s);
    }

    return out;
}

// need to think about a way to not copy data field unnessecarily
// or perhaps that needs to be read in after the fact
optional<Request> parse_request(char const* msg) {
    auto tokens = split(msg, ' ');
    if (tokens.size() < 3) {
        return {};
    }

    string cmd = tokens[0];

    // insert username validation
    string username = tokens[1];
    if (username.size() > FS_MAXUSERNAME) {
        return {};
    }

    // path validation
    if (tokens[2].at(0) != '/'
        || (tokens[2].back() == '/' && tokens[2].size() != 1)) {
        return {};
    }

    if (tokens[2].size() > FS_MAXPATHNAME) {
        return {};
    }

    // get rid of first character
    Path path = split(tokens[2].c_str() + 1, '/');

    // check filename at end
    if (path.size()) {
        for (size_t i = 0; i < path.size(); i++) {
            if (path[i].size() > FS_MAXFILENAME) {
                return {};
            }
        }
    }

    // string not compatible with switch statement
    if (cmd == "FS_READBLOCK" && tokens.size() == 4) {
        try {
            if (tokens[3].at(0) == '0' && tokens[3].size() != 1) {
                // block should be canonical base 10
                return {};
            }
            uint32_t block = std::stoul(tokens[3]);
            if (block > FS_MAXFILEBLOCKS) {
                return {};
            }
            return FS_READBLOCK { username, path, block };
        }
        catch (std::invalid_argument& e) {
            return {};
        }
    }
    else if (cmd == "FS_WRITEBLOCK" && tokens.size() == 4) {
        try {
            if (tokens[3].at(0) == '0' && tokens[3].size() != 1) {
                // block should be canonical base 10
                return {};
            }
            uint32_t block = std::stoul(tokens[3]);
            if (block > FS_MAXFILEBLOCKS) {
                return {};
            }
            return FS_WRITEBLOCK { username, path, block, nullptr };
        }
        catch (std::invalid_argument& e) {
            return {};
        }
    }
    else if (cmd == "FS_CREATE" && tokens.size() == 4) {
        // verify f or d
        if (tokens[3] != "f" && tokens[3] != "d") {
            return {};
        }
        return FS_CREATE { username, path, tokens[3].front() };
    }
    else if (cmd == "FS_DELETE" && tokens.size() == 3) {
        return FS_DELETE { username, path };
    }
    else {
        return {};
    }
}
