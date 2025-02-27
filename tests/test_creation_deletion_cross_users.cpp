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
    status = fs_create("user1", "/first", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user2", "/first", 'f');
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_delete("user2", "/first");
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_delete("user1", "/first");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user2", "/first", 'f');
    std::cout << "status of user2 create correct request " << status << std::endl;

    // check with reads
    char readdata[FS_BLOCKSIZE];
    status = fs_readblock("user1", "/first", 0, readdata);
    std::cout << "status of incorrect request " << status << std::endl;
    std::cout << readdata << std::endl;


    status = fs_writeblock("user2", "/first", 0, readdata);
    std::cout << "status of correct request " << status << std::endl;
    std::cout << readdata << std::endl;


    status = fs_readblock("user2", "/first", 0, readdata);
    std::cout << "status of correct request " << status << std::endl;
    std::cout << readdata << std::endl;


    status = fs_writeblock("user1", "/first", 0, readdata);
    std::cout << "status of incorrect request " << status << std::endl;
    std::cout << readdata << std::endl;



    return 0;
}
