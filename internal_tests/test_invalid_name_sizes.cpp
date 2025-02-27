
#include <iostream>

#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }

    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    int status;
    status = fs_create("user_too_too_long", "/first", 'f');
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_create(
      "user",
      "/filename_far_far_far_too_long_way_way_way_way_way_way_way_way_way_way", 'f');
    std::cout << "status of incorrect request " << status << std::endl;


    auto send_info = "this is some data to write";

    // status = fs_writeblock("name", "path",  0, send_info);

    status = fs_writeblock(
      "user1",
      "/path/"
      "pathname_too_long_far_far_far_too_long_way_way_way_way_way_way_way_way_way_"
      "way_far_far_far_too_long_way_way_way_way_way_way_way_way_way_way",
      1, send_info);
    std::cout << "status of incorrect request " << status << std::endl;

    return 0;
}
