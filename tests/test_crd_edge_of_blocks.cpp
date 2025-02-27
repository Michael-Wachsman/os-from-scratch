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

    int status;
    status = fs_create("user1", "/first", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/place", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/place/one", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/place/two", 'd');
    std::cout << "status of correct request " << status << std::endl;

    // fill two to edge with files
    for (int i = 0; i < 8; i++) {
        status = fs_create(
          "user1", ("/place/two/" + std::to_string(i) + ".file").c_str(), 'f');
        std::cout << "status of correct request " << status << std::endl;
    }

    // make a populated directory
    status = fs_create("user1", "/place/two/dir", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/place/two/dir/another", 'f');
    std::cout << "status of correct request " << status << std::endl;


    status = fs_delete("user1", "/place/two/dir");
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_delete("user1", "/place/two/dir/another");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_delete("user1", "/place/two/dir");
    std::cout << "status of correct request " << status << std::endl;

    // do similar on the other side, with a file and directories instead
    for (int i = 0; i < 16; i++) {
        status = fs_create(
          "user1", ("/place/one/" + std::to_string(i) + "_dir").c_str(), 'd');
        std::cout << "status of correct request " << status << std::endl;
    }

    status = fs_create("user1", "/place/two/file.exists", 'f');
    std::cout << "status of correct request " << status << std::endl;

    for (int i = 0; i < 16; i++) {
        status = fs_writeblock("user1", "/place/two/file.exists", i, writedata);
        std::cout << "status of correct request writing .exists" << status << std::endl;
    }

    // deletion wtih freeing all blocks
    status = fs_delete("user1", "/place/two/file.exists");
    std::cout << "status of correct request delete .exists" << status << std::endl;

    return 0;
}
