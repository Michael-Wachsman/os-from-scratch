
#include <array>
#include <iostream>
#include <string>

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
    status = fs_create("user1", "/first", 'd');
    std::cout << "status of correct request " << status << std::endl;

    // 124 max blocks in a file or directory
    // 4096 blocks available on disk

    std::array<char, FS_BLOCKSIZE> a_s { 'a' };

    status = fs_create("user1", "/first", 'd');
    std::cout << "status of correct request " << status << std::endl;

    if(status != -1){
        for (int i = 0; i < 124 * 8; ++i) {
            auto name = "/first/" + std::to_string(i) + ".txt";
            status = fs_create("user1", name.c_str(), 'f');
            std::cout << "status of correct request " << status << std::endl;
            for (int j = 0; j < 124; ++j) {
                status = fs_writeblock("user1", name.c_str(), j, a_s.data());
                std::cout << "status of correct request " << status << std::endl;
            }
        }
    }


    status = fs_create("user1", "/second", 'd');
    std::cout << "status of correct request " << status << std::endl;

    return 0;
}
