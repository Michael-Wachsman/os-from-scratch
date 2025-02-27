#include <cassert>
#include <iostream>
#include <optional>
#include <vector>

#include "parsing.h"

std::ostream& operator<<(std::ostream& os, vector<string> const& lst) {
    os << "[";
    for (auto elem : lst) {
        os << elem << ",";
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, optional<Request> const& maybe_req) {
    if (maybe_req.has_value()) {
        os << maybe_req.value();
    }
    else {
        os << "{}";
    }
    return os;
}


template <typename T>
void assert_eq(T const& lhs, T const& rhs) {
    // yes, != is not implemented on some stuff
    if (!(rhs == lhs)) {
        std::cout << "Not equal:\n";
        std::cout << "lhs: " << lhs << std::endl;
        std::cout << "rhs: " << rhs << std::endl;
    }
}


int main(int argc, char* argv[]) {
    // SPLIT
    assert_eq(split("a b c d", ' '), vector<string>({ "a", "b", "c", "d" }));
    assert_eq(split("user/bin/hope", '/'), { "user", "bin", "hope" });
    assert_eq(split("/user/bin/hope", '/'), { "", "user", "bin", "hope" });
    // note behavior is to ignore trailing
    assert_eq(split("/user/bin/hope/", '/'), { "", "user", "bin", "hope" });

    assert_eq(split("/", '/'), { "" });
    assert_eq(split("", '/'), {});


    // PARSE REQUEST
    assert_eq(
      parse_request("FS_READBLOCK user /path/next 10"),
      optional { Request { FS_READBLOCK { "user", { "path", "next" }, 10 } } });

    assert_eq(parse_request("FS_WRITEBLOCK user /path/other 0"),
              optional { Request {
                FS_WRITEBLOCK { "user", { "path", "other" }, 0, nullptr } } });
    assert_eq(
      parse_request("FS_CREATE user /path/next f"),
      optional { Request { FS_CREATE { "user", { "path", "next" }, 'f' } } });
    assert_eq(parse_request("FS_DELETE rwchua /caen/tetris"),
              optional { Request { FS_DELETE { "rwchua", { "caen", "tetris" } } } });

    // might want to change behavior of this
    assert_eq(parse_request("FS_DELETE user /"),
              optional { Request { FS_DELETE { "user", {} } } });

    // PARSE REQUEST ERRORS
    assert_eq(parse_request(""), {});
    assert_eq(parse_request("abc"), {});
    assert_eq(parse_request("I chose hope and ended up"), {});
    assert_eq(parse_request("FS_READBLOCK user // 0"), {});
    assert_eq(parse_request("FS_READBLOCK 12345678910 /bingCh/Ling 0"), {});
    assert_eq(parse_request("FS_READBLOCK user2 /does_not_exist/ 0"), {});
    assert_eq(parse_request("FS_DELETE user /path/next 10"), {});
    assert_eq(parse_request("FS_WRITEBLOCK user path/next 10"), {});
    assert_eq(parse_request("FS_WRITEBLOCK user /path/next/ 10"), {});
    assert_eq(parse_request("FS_WRITEBLOCK user /path/next cappa"), {});
    assert_eq(parse_request("FS_CREATE user /path/next k"), {});
    assert_eq(
      parse_request(
        "FS_CREATE user "
        "/path/next/ACDEFGHJMOPSUVWXYZabcdfhklmnouhduohesuhdouhiuwfhfiuwhdddeuuui/"
        "hi f"),
      {});
    assert_eq(parse_request("FS_READBLOCK user / 0"), {});
    assert_eq(parse_request("FS_READBLOCK"), {});
    assert_eq(parse_request("FS_WRITEBLOCK"), {});
    assert_eq(parse_request("FS_CREATE"), {});
    assert_eq(parse_request("FS_DELETE"), {});
    assert_eq(parse_request("FS_READBLOCK user /this/that -1.2"), {});
    // assert_eq(parse_request("FS_READBLOCK user /this/that 12.2"), {});
    assert_eq(parse_request("FS_READBLOCK user /this/that 002"), {});


    // too long username, filename, pathname, blocks
    assert_eq(parse_request("FS_CREATE user_too_too_long /path/next f"), {});
    assert_eq(
      parse_request(
        "FS_CREATE user "
        "/filename_far_far_far_too_long_way_way_way_way_way_way_way_way_way_way f"),
      {});
    assert_eq(
      parse_request(
        "FS_CREATE user "
        "/path/"
        "pathname_too_long_far_far_far_too_long_way_way_way_way_way_way_way_way_way_"
        "way_far_far_far_too_long_way_way_way_way_way_way_way_way_way_way f"),
      {});
    assert_eq(
      parse_request(
        "FS_WRITEBLOCK user "
        "/path/"
        "pathname_too_long_far_far_far_too_long_way_way_way_way_way_way_way_way_way_"
        "way_far_far_far_too_long_way_way_way_way_way_way_way_way_way_way  1"),
      {});
    // not handled by this
    //  assert_eq(parse_request("FS_WRITEBLOCK user /path/other 125"), {});

    std::cout << "Finished all tests!" << std::endl;
    return 0;
}
