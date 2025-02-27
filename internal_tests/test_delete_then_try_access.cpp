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

    std::string content
      = "This is a bruh moment, "
        "123456789101112131415161718192021222324252627282930313233343536373839404142"
        "434445464748495051525354555657585960616263466566676869707172737475767778798"
        "081828384858687888990919239495599697899100This is a bruh moment, "
        "123456789101112131415161718192021222324252627282930313233343536373839404142"
        "434445464748495051525354555657585960616263466566676869707172737475767778798"
        "081828384858687888990919239495599697899100This is a bruh moment, "
        "123456789101112131415161718192021222324252627282930313233343536373839404142"
        "434445464748495051525354555657585960616263466566676869707172737475767778798"
        "081828384858687888990919239495599697899100";

    int status;

    status = fs_create("user1", "/layer1", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/layer1/layer2", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/layer1/layer2/layer3", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/layer1/layer2/layer3/bruh.txt", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status
      = fs_writeblock("user1", "/layer1/layer2/layer3/bruh.txt", 0, content.c_str());
    std::cout << "status of correct request " << status << std::endl;

    status = fs_delete("user1", "/layer1/layer2/layer3/bruh.txt");
    std::cout << "status of correct request " << status << std::endl;

    std::cout << "Content is still '" << content << "'" << std::endl;

    status
      = fs_writeblock("user1", "/layer1/layer2/layer3/bruh.txt", 0, content.c_str());
    std::cout << "status of incorrect request " << status << std::endl;

    std::cout << "Content should still be '" << content << "'" << std::endl;

    return 0;
}
