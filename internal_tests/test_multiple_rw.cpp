#include <iostream>
#include <thread>

#include "fs_client.h"

void reader() {
    // read
    char readdata[FS_BLOCKSIZE];
    int status = 0;

    for (int i = 0; i < 30; ++i) {
        status = fs_readblock(
          "user1", ("/first/" + std::to_string(i) + ".txt").c_str(), 0, readdata);
        if (status == -1) {
            std::cout << "status of correct request " << status << std::endl;
        }
    }

    for (int i = 0; i < 15; ++i) {
        status = fs_readblock(
          "user1", ("/second/" + std::to_string(i) + ".txt").c_str(), 0, readdata);
        if (status == -1) {
            std::cout << "status of correct request " << status << std::endl;
        }
    }
}

void writer() {
    char const* diffdata
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

    int status = 0;

    for (int i = 0; i < 30; i += 2) {
        status = fs_writeblock(
          "user1", ("/first/" + std::to_string(i) + ".txt").c_str(), 1, diffdata);
        if (status == -1) {
            std::cout << "status of correct request " << status << std::endl;
        }
    }

    for (int i = 0; i < 15; i += 3) {
        status = fs_writeblock(
          "user1", ("/second/" + std::to_string(i) + ".txt").c_str(), 0, diffdata);
        if (status == -1) {
            std::cout << "status of correct request " << status << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }

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

    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    int status;
    status = fs_create("user1", "/first", 'd');
    std::cout << "status of correct create request " << status << std::endl;

    status = fs_create("user1", "/second", 'd');
    std::cout << "status of correct create request " << status << std::endl;

    for (int i = 0; i < 30; ++i) {
        status = fs_create("user1", ("/first/" + std::to_string(i) + ".txt").c_str(),
                           'f');
        if (status == -1) {
            std::cout << "status of correct request " << status << std::endl;
        }

        status = fs_writeblock(
          "user1", ("/first/" + std::to_string(i) + ".txt").c_str(), 0, writedata);
        if (status == -1) {
            std::cout << "status of correct request " << status << std::endl;
        }
    }


    for (int i = 0; i < 15; ++i) {
        status = fs_create("user1",
                           ("/second/" + std::to_string(i) + ".txt").c_str(), 'f');
        if (status == -1) {
            std::cout << "status of correct request " << status << std::endl;
        }

        status = fs_writeblock(
          "user1", ("/second/" + std::to_string(i) + ".txt").c_str(), 0, writedata);
        if (status == -1) {
            std::cout << "status of correct request " << status << std::endl;
        }
    }

    std::thread t0(reader);
    std::thread t1(reader);
    std::thread tw1(writer);
    std::thread t2(reader);
    std::thread t3(reader);
    std::thread tw4(writer);
    std::thread t4(reader);
    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    tw1.join();
    tw4.join();

    return 0;
}
