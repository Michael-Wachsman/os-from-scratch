#include <iostream>

#include "fs_client.h"
#include "fs_param.h"

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

    // has a specific file to make output shorter for AG

    int status;
    status = fs_create("user1", "/first", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/second", 'd');
    std::cout << "status of correct request " << status << std::endl;


    status = fs_create("user1", "/first/a", 'd');
    std::cout << "status of correct request " << status << std::endl;

    char arr[FS_BLOCKSIZE];
    status = fs_readblock("user1", "/first/a", 124, arr);
    std::cout << "status of incorrect request " << status << std::endl;

    status = fs_create("user1", "/first/b", 'd');
    std::cout << "status of correct request " << status << std::endl;


    // fill a directory _exactly_, then make sure the subsequent one fails
    // inode can hold 124, each block has 8 direntries
    // aka if creates went through, and we're redoing this from scratch
    if (status == 0) {
        for (int i = 0; i < 8 * (124 / 2); ++i) {
            // make a dir and a file each time, in case weird issues
            status = fs_create(
              "user1", ("/first/a/" + std::to_string(2 * i) + "_dir").c_str(), 'd');
            if (status == -1) {
                std::cout << "incorrectly failed to create a dir inside"
                          << std::endl;
            }

            status = fs_create(
              "user1", ("/first/a/" + std::to_string(2 * i + 1) + ".text").c_str(),
              'f');
            if (status == -1) {
                std::cout << "incorrectly failed to create a file inside"
                          << std::endl;
            }
        }
    }


    // this is an issue
    status = fs_create("user1", "/first/a/too_much.text", 'd');
    std::cout << "status of incorrect request creating in full dir " << status
              << std::endl;

    // do same thing in writing to a block
    status = fs_create("user1", "/big_file.txt", 'f');
    char const* data
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

    for (int i = 0; i < 124; ++i) {
        status = fs_writeblock("user1", "/big_file.txt", i, data);
        if (status == -1) {
            std::cout << "failed to write to blocks" << std::endl;
        }
    }


    status = fs_writeblock("user1", "/big_file.txt", 124, data);
    std::cout << "status of incorrect request creating 125th block " << status
              << std::endl;

    status = fs_delete(
      "user1", ("/first/a/" + std::to_string(2 * 5 + 1) + ".text").c_str());
    std::cout << "status of del request " << status << std::endl;

    status = fs_create("user1", "/first/a/dremp.text", 'f');
    std::cout << "status of new write request " << status << std::endl;

    status = fs_create("user1", "/first/a/drlep.text", 'f');
    std::cout << "status of bad new write request " << status << std::endl;


    return 0;
}
