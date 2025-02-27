#include <cassert>
#include <cstdlib>
#include <iostream>

#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

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

    char const* extrawritedata
      = "'MURICA!!!! *EAGLE NOISES "
        "CAWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW "
        "CAWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW "
        "CAWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW "
        "CAWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW "
        "CAWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW*";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    status = fs_create("user1", "/bruh.txt", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/first", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/first/bruh.txt", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_writeblock("user1", "/first/bruh.txt", 0, writedata);
    std::cout << "status of correct request " << status << std::endl;

    status = fs_writeblock("user1", "/first/bruh.txt", 1, extrawritedata);
    std::cout << "status of correct request " << status << std::endl;

    status = fs_readblock("user1", "/first/bruh.txt", 1, readdata);
    std::cout << "status of correct request " << status << std::endl;

    std::cout << "Contents of readdata '" << readdata << "'" << std::endl;
}