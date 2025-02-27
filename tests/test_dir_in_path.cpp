#include <array>
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
    status = fs_create("user1", "/first", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/first/file", 'f');
    std::cout << "status of correct request " << status << std::endl;

    std::array<char, FS_BLOCKSIZE> send_info {};

    // create some unnessecary disk reads
    status = fs_writeblock("user1", "/first/file", 0, send_info.data());
    std::cout << "status of correct request " << status << std::endl;

    status = fs_writeblock("user1", "/first/file", 1, send_info.data());
    std::cout << "status of correct request " << status << std::endl;

    status = fs_writeblock("user1", "/first/file", 2, send_info.data());
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/first/file/name.txt", 'f');
    std::cout << "status of incorrect request " << status << std::endl;

    // let's check it for all operations
    status = fs_create("user1", "/first/file/file/name.txt", 'f');
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_delete("user1", "/first/file/file/name.txt");
    std::cout << "status of incorrect request " << status << std::endl;

    char readdata[FS_BLOCKSIZE];
    status = fs_readblock("user1", "/first/file/file/name.txt", 0, readdata);
    std::cout << "status of incorrect request " << status << std::endl;

    std::array<char, FS_BLOCKSIZE> senddata {};
    status = fs_writeblock("user1", "/first/file/file/name.txt", 0, senddata.data());
    std::cout << "status of incorrect request " << status << std::endl;
    return 0;
}
