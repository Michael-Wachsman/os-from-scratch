
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


    status = fs_create("user1", "/first/boo", 'f');
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_create("user1", "/second", 'd');
    std::cout << "status of correct request " << status << std::endl;
    //
    status = fs_create("user1", "/second/next", 'd');
    std::cout << "status of correct request " << status << std::endl;

    //
    status = fs_create("user1", "/second/another", 'd');
    std::cout << "status of correct request " << status << std::endl;
    //
    //
    status = fs_create("user1", "/second/another/file", 'f');
    std::cout << "status of correct request " << status << std::endl;

    return 0;
}
