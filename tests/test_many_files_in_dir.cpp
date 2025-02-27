#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

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

    fs_create("toafu", "/dir", 'd');

    for (int i = 0; i < 16; ++i) {
        std::string filename = "/dir/file" + std::to_string(i) + ".txt";
        int status = fs_create("toafu", filename.c_str(), 'f');
        std::cout << "status of correct request " << status << std::endl;
    }

    fs_writeblock("toafu", "/dir/file0.txt", 0, "I am file0.");

    fs_writeblock("toafu", "/dir/file1.txt", 0,
                  "These are a few of my favorite things.");

    fs_writeblock("toafu", "/dir/file1.txt", 1, "Tostitos");

    for (int i = 14; i >= 0; i -= 2) {
        std::string filename = "/dir/file" + std::to_string(i) + ".txt";
        fs_delete("toafu", filename.c_str());
    }

    // Only odd filenames remain
    fs_delete("toafu", "/dir/file3.txt");
    fs_delete("toafu", "/dir/file1.txt");
    fs_delete("toafu", "/dir/file7.txt");
    fs_delete("toafu", "/dir/file5.txt");

    fs_writeblock(
      "toafu", "/dir/file11.txt", 0,
      "NONONONONONONONONONONONONONONO I'M SORRY DUOLINGO I ACCIDENTALLY PUT THE "
      "WRONG KANJI FOR 'WATER' PLEASE CAN I ASK FOR ANY FORGIVENESS WHATSOEVER "
      "OHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGOD"
      "OHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGOD YOU CAN TAKE "
      "THE KIDS JUST LEAVE ME ALONE "
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA *inahle* "
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

    int status = fs_readblock("toafu", "/dir/file3.txt", 0, nullptr);
    if (status == -1) {
        printf("This file no longer exists\n");
    }
    else {
        printf("Somehow you accessed the deleted file that was previously here\n");
    }
}
