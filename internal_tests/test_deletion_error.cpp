
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

    status = fs_delete("user1", "/first");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_delete("user1", "/first");
    std::cout << "status of incorrect request " << status << std::endl;

    // deleting root directory
    status = fs_delete("user1", "/");
    std::cout << "status of incorrect request " << status << std::endl;

    // deleting something with subdirectories
    status = fs_create("user1", "/first", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/first/second", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_delete("user1", "/first");
    std::cout << "status of incorrect request " << status << std::endl;

    // deleting someone else's stuff
    status = fs_create("user2", "/second", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_delete("user1", "/second");
    std::cout << "status of incorrect request " << status << std::endl;
    //
    // this is failing for some reason?
    // test deleting where the parent is a file
    status = fs_create("user2", "/third", 'f');
    std::cout << "status of correct request " << status << std::endl;

    //  give it some blocks
    std::array<char, FS_BLOCKSIZE> write_data {};
    for (int i = 0; i < 3; ++i) {
        status = fs_writeblock("user2", "/third", i, write_data.data());
        std::cout << "status of correct request " << status << std::endl;
    }

    status = fs_delete("user2", "/third/name.txt");
    std::cout << "status of incorrect request " << status << std::endl;
    
    return 0;
}
