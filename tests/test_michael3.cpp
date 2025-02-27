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

    std::vector<std::string> names = { "/Hi", "/Human", "/Im", "/Sans" };

    for (int i = 0; i < 4; i++) {
        status = fs_create("002", names[i].c_str(), 'd');
        std::cout << "status of correct request " << status << std::endl;
        if (status != 0) {
            break;
        }
        for (int j = 0; j < 124 * 8; j++) {
            std::string filename = names[i] + "/" + std::to_string(j);
            status = fs_create("002", filename.c_str(), 'f');
            std::cout << "status of correct request " << status << std::endl;
            if (status == -1) {
                break;
            }
        }
        if (i == 3) {
            return 0;
        }
    }

    std::string filename = "/Hi/" + std::to_string(3);
    status = fs_writeblock("002", filename.c_str(), 0, writedata);
    std::cout << "status of correct request " << status << std::endl;
    
    std::string filelame = "/Im/" + std::to_string(3);

    fs_delete("002", filelame.c_str());
    std::cout << "status of correct request " << status << std::endl;

    fs_delete("002", filename.c_str());
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("002", filename.c_str(), 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_writeblock("002", filename.c_str(), 0, writedata);
    std::cout << "status of correct request " << status << std::endl;

    char readdata[FS_BLOCKSIZE];


    status = fs_readblock("002", filename.c_str(), 0, readdata);
    std::cout << "status of correct request " << status << std::endl;
    std::cout << readdata << std::endl;


    return 0;
}