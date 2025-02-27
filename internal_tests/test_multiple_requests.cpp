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
    status = fs_create("user1", "/first", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_delete("user1", "/first");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_readblock("user2", "/does_not_exist/", 0, nullptr);
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_create("user1", "/later", 'f');
    std::cout << "status of correct request " << status << std::endl;

    auto send_info = "this is some data to write";

    char senddata[FS_BLOCKSIZE];
    status = fs_writeblock("user1", "/later", 0, send_info);
    std::cout << "status of correct request " << status << std::endl;
    return 0;
}
