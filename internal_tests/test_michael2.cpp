
#include <iostream>
#include <string>

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
    status = fs_delete("002", "/HI");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("002", "/HI", 'd');
    std::cout << "status of correct request " << status << std::endl;


    status = fs_create("002", "/HI/Jello", 'f');
    std::cout << "status of correct request " << status << std::endl;

    std::cout << "HI BRO" << std::endl;
    status = fs_writeblock("002", "/HI/Jello", 0, writedata);
    std::cout << "status of correct request " << status << std::endl;


    char readdata[FS_BLOCKSIZE];

    // status = fs_readblock("002", "/HI/Jello", 0, readdata);
    // std::cout << "status of correct request " << status << std::endl;

    // status = fs_readblock("002", "/HI/Jello", 9, readdata);
    // std::cout << "status of correct request " << status << std::endl;
    // std::cout << readdata << std::endl;

    status = fs_delete("002", "/HI/Jello");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_writeblock("002", "/HI", 1, writedata);
    std::cout << "status of correct request " << status << std::endl;

    status = fs_delete("002", "/HI");
    std::cout << "status of correct request " << status << std::endl;

    status = fs_delete("002", "/HI");
    std::cout << "status of correct request " << status << std::endl;


    return 0;
}
