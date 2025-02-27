#include <cassert>
#include <cstdlib>
#include <iostream>

#include "fs_client.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

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
    char const* uhoh
      = "HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN "
        "INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY "
        "EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! "
        "HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN "
        "INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY "
        "EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! "
        "HAHA! NOBODY EXPECTS THE ELVEN INQUISITION! HAHA! NOBODY EXPECTS THE E\n";

    char readdata[FS_BLOCKSIZE];

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    int status = fs_create("toafu", "/dir/data.bin", 'f');
    cout << "status of request " << status << endl;

    status = fs_create("toafu", "/test.txt", 'f');
    cout << "status of request " << status << endl;


    int check = fs_create("toafu", "/dir", 'd');
    if (check == -1) {
        cout << "Can't create directory that already exists" << endl;
    }
    else {
        cout << "/dir must have never been made" << endl;
    }

    check = fs_create("toafu", "/dir/file0.txt", 'f');
    if (check == -1) {
        cout << "Can't create file that already exists" << endl;
    }
    else {
        cout << "/dir/file0.txt must have never been made" << endl;
    }

	check = fs_create("toafu", "/dir/file0.txt", 'd');
    if (check == -1) {
        cout << "Can't create directory that is already a file" << endl;
    }
    else {
        cout << "/dir/file0.txt must have never been made" << endl;
    }

    check = fs_writeblock("toafu", "/dir/file0.txt", 2, subscribedata);
    if (check == -1) {
        cout << "Can't write past the EOF block!" << endl;
    }
    else {
        cout << "How did you write past the EOF block?! O_O" << endl;
    }

    check = fs_writeblock("BADDOG", "/dir/file2.txt", 0, uhoh);
    if (check == -1) {
        cout << "Can't write to a directory you don't own!" << endl;
    }
    else {
        cout << "So you may have taken EECS 388..." << endl;
    }

    check = fs_writeblock("blobowoevil", "/dir/file0.txt", 0, notsubscribedata);
    if (check == -1) {
        cout << "Invalid username!" << endl;
    }
    else {
        cout << "You are more powerful than I imagined..." << endl;
    }

    check = fs_writeblock("toafu", "/din/data.bin", 0, uhoh);
    if (check == -1) {
        cout << "Can't write to a directory that doesn't exist!" << endl;
    }
    else {
        cout << "The Law of Conversation of Mass has been broken." << endl;
    }

    fs_readblock("toafu", "/dir/file0.txt", 0, readdata);

    std::cout << readdata << std::endl;

	check = fs_delete("toafu", "/dir/file15.txt");
    if (check == -1) {
        cout << "This file exists yet it somehow could not be deleted" << endl;
    }
    else {
        cout << "Successful delete of pre-made /dir/file15.txt" << endl;
    }
}