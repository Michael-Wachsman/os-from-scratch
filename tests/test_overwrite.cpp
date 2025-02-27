
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

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

    char const* notsubscribedata
      = "YouTube analytics show that 99% of you are not yet subscribed! What are "
        "you waiting for?!?! Subscribing is free and it supports the channel! "
        "You're already coming back anyways, so why not click the button and the "
        "bell? Get notified today! Use code TOAFU at checkout! Use code TOAFU at "
        "checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code "
        "TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! "
        "Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at "
        "c\n";

    char const* subscribedata
      = "YouTube analytics show that 1% of you are subscribed! You are the elites! "
        "You are the chosen ones! The worthy! Thank you for your continued support "
        "- whether it's just watching the video, or leaving a like or comment, I "
        "appreciate it so much. Use code TOAFU at checkout! Use code TOAFU at "
        "checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! Use code "
        "TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU at checkout! "
        "Use code TOAFU at checkout! Use code TOAFU at checkout! Use code TOAFU a";

    char readdata[FS_BLOCKSIZE];


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
    status = fs_delete("002", "/HI");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("002", "/HI", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_writeblock("002", "/HI", 0, writedata);
    std::cout << "status of correct request " << status << std::endl;

    status = fs_readblock("002", "/HI", 0, readdata);
    std::cout << "status of correct request " << status << std::endl;
    std::cout << readdata << std::endl;

    status = fs_writeblock("002", "/HI", 0, notsubscribedata);
    std::cout << "status of correct request " << status << std::endl;

    status = fs_readblock("002", "/HI", 0, readdata);
    std::cout << "status of correct request " << status << std::endl;
    std::cout << readdata << std::endl;

    status = fs_delete("002", "/HI");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_readblock("002", "/HI", 0, readdata);
    std::cout << "status of correct request " << status << std::endl;
    std::cout << readdata << std::endl;

    return 0;
}
