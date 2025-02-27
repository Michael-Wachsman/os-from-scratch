#include <iostream>
#include <thread>

#include "fs_client.h"

void child(char* hostname, int port, int num) {
    fs_clientinit(hostname, port);
    int status;
    status = fs_create("user1", "/first", 'f');
    std::cout << "status of correct create request " << status << std::endl;

    status = fs_delete("user1", "/first");
    std::cout << "status of correct delete request " << status << std::endl;

    status = fs_readblock("user2", "/does_not_exist/", num, nullptr);
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_create("user1", "/later", 'f');
    std::cout << "status of correct later create request " << status << std::endl;

    auto send_info = "this is some data to write";

    char senddata[FS_BLOCKSIZE];
    status = fs_writeblock("user1", "/later", num, send_info);
    std::cout << "status of correct write request " << status << std::endl;
}

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }

    server = argv[1];
    server_port = atoi(argv[2]);
    std::thread t(child, server, server_port, 0);
    std::thread t1(child, server, server_port, 1);
    std::thread t2(child, server, server_port, 2);

    t.join();
    t1.join();
    t2.join();

    return 0;
}
