#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *notsubscribedata = "YouTube analytics show that 99% of you are not yet subscribed! What are you waiting for?!?! Subscribing is free and it supports the channel! You're already coming back anyways, so why not click the button and the bell? Get notified today! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at c\n";
    const char *subscribedata = "YouTube analytics show that 1% of you are subscribed! You are the elites! You are the chosen ones! The worthy! Thank you for your continued support - whether it's just watching the video, or leaving a like or comment, I appreciate it so much. Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU a";
    const char *uhoh = "HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE E\n";

    char readdata[FS_BLOCKSIZE];

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_create("toafu", "/test.txt", 'f');

    if (fs_writeblock("toafu", "/dir/file0.txt", 1, notsubscribedata) == 0) {
        printf("Nonempty fs file grow works\n");
    } else {
        printf("nononononono\n");
    }

    fs_writeblock("toafu", "/dir/file12.txt", 0, notsubscribedata);

    fs_writeblock("toafu", "/dir/file12.txt", 1, subscribedata);

    fs_readblock("toafu", "/dir/file12.txt", 1, readdata);

    std::cout << readdata << std::endl;

    fs_readblock("toafu", "/dir/file12.txt", 0, readdata);

    std::cout << readdata << std::endl;

    fs_writeblock("toafu", "/dir/file12.txt", 0, uhoh);

    fs_readblock("toafu", "/dir/file12.txt", 0, readdata);

    std::cout << readdata << std::endl;

    fs_readblock("toafu", "/dir/file1.txt", 0, readdata);

    std::cout << readdata << std::endl;
}