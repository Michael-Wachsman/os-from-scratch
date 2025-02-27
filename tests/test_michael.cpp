
#include <iostream>

#include "fs_client.h"

int main(int argc, char* argv[]) {
    char const* writedata
      = "We hold these truths to be self-evident, that all men are created equal, "
        "that they are endowed by their Creator with certain unalienable Rights, "
        "that among these are Life, Liberty and the pursuit of Happiness. -- That "
        "to secure these rights, Governments are instituted among Men, deriving "
        "their just powers from the consent of the governed, -- That whenever any "
        "Form of Government becomes destructive of these ends, it is the Right of "
        "the People to alter or to abolish it, and to institute new Government, "
        "laying its foundation on such principles and organizing its powers in such "
        "form, as to them shall seem most likely to effect their Safety and "
        "Happiness.";

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

    status = fs_create("user1", "/first", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/first/second", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user10", "/first", 'd');
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_create("user10", "/second", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user10", "/second/pogg", 'f');
    std::cout << "status of correct request " << status << std::endl;


    for (int i = 0; i < 15; i++) {
        auto second_path = "/second/" + std::to_string(i);
        if (i % 7 == 0) {
            status = fs_create("user10", second_path.c_str(), 'f');
            status = fs_writeblock("user10", second_path.c_str(), 0, "sheesh");
        }
        else if (i % 4 == 0) {
            status = fs_create("user10", second_path.c_str(), 'd');
        }
        else {
            status = fs_writeblock("user10", "/second/pogg", i, writedata);
        }
    }

    char readdata[FS_BLOCKSIZE];


    status = fs_readblock("user1", "/second/pogg", 4, readdata);

    status = fs_readblock("user10", "/second/pogg", 10, readdata);


    status = fs_delete("user1", "/second");
    std::cout << "status of incorrect request " << status << std::endl;

    return 0;
}
