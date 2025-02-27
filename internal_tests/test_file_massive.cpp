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

    std::string nonono
      = "NONONONONONONONONONONONONONONO I'M SORRY DUOLINGO I ACCIDENTALLY PUT THE "
        "WRONG KANJI FOR 'WATER' PLEASE CAN I ASK FOR ANY FORGIVENESS WHATSOEVER "
        "OHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYG"
        "ODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGODOHMYGOD YOU CAN "
        "TAKE THE KIDS JUST LEAVE ME ALONE "
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA *inahle* "
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

    std::string yay
      = "yayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayay"
        "ayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayaya"
        "yayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayay"
        "ayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayaya"
        "yayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayay"
        "ayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayayaya"
        "yayayayayayayayayayayayayayayayayaayayayayayayayayayayayayaya";

    fs_create("toafu", "/file.txt.bmp.osz.ipynb.shampoo", 'f');

    for (unsigned int i = 0; i < 123; ++i) {
        fs_writeblock("toafu", "/file.txt.bmp.osz.ipynb.shampoo", i, nonono.c_str());
    }

    char readdata[FS_BLOCKSIZE];

    if (fs_writeblock("toafu", "/file.txt.bmp.osz.ipynb.shampoo", 123, yay.c_str())
        == -1) {
        printf("File too big!\n");
    }
    else {
        printf("wrote to last block\n");
    }

	if (fs_writeblock("toafu", "/file.txt.bmp.osz.ipynb.shampoo", 124, yay.c_str())
        == -1) {
        printf("File too big to add to!\n");
    }
    else {
        printf("umich, we have a problem. a write problem.\n");
	}

    if (fs_readblock("toafu", "/file.txt.bmp.osz.ipynb.shampoo", 124, readdata)
        == -1) {
        printf("File too big!\n");
    }
    else {
        printf("umich, we have a problem. a read problem.\n");
    }

    fs_readblock("toafu", "/file.txt.bmp.osz.ipynb.shampoo", 123, readdata);
    std::cout << readdata << std::endl;
}
