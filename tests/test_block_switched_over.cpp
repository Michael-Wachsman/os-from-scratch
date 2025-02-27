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


    // create direntries one at a time in order
    for (int i = 0; i < 50; ++i) {
        auto name = "/" + std::to_string(i) + ".txt";
        status = fs_create("user1", name.c_str(), 'f');
        if (status == -1) {
            std::cout << "failed a create" << std::endl;
        }
    }

    // delete direntries one at a time backwards, ensuring blocks shift back properly
    // Actyually lets do it kinda weird

    for (int i = 0; i < 50; i+=2) {
        auto name = "/" + std::to_string(i * 2) + ".txt";
        fs_delete("user1", name.c_str());
        if (status == -1) {
            std::cout << "failed a delete" << std::endl;
        }
    }
    for (int i = 1; i < 50; i+=2) {
        auto name = "/" + std::to_string(i * 2) + ".txt";
        fs_delete("user1", name.c_str());
        if (status == -1) {
            std::cout << "failed a delete" << std::endl;
        }
    }

    return 0;
}
