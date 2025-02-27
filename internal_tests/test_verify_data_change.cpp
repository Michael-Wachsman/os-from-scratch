#include <array>
#include <cstdio>
#include <iostream>

#include "fs_client.h"
#include "fs_param.h"

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

    status = fs_create("user1", "/mine", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/mine/first", 'f');
    std::cout << "status of correct request " << status << std::endl;


    std::array<char, FS_BLOCKSIZE> a_string { 'a' };
    a_string.back() = '\0';

    std::array<char, FS_BLOCKSIZE> b_string { 'b' };
    b_string.back() = '\0';

    status = fs_writeblock("user1", "/mine/first", 0, a_string.data());
    std::cout << "status of correct request " << status << std::endl;

    std::array<char, FS_BLOCKSIZE> readdata;

    status = fs_readblock("user1", "/mine/first", 0, readdata.data());
    std::cout << "status of correct request " << status << std::endl;
    printf("should be a's %s\n", readdata.data());

    status = fs_writeblock("user1", "/mine/first", 0, b_string.data());
    std::cout << "status of correct request " << status << std::endl;

    status = fs_readblock("user1", "/mine/first", 0, readdata.data());
    std::cout << "status of correct request " << status << std::endl;
    printf("should be b's %s\n", readdata.data());

    return 0;
}
