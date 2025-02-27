
#include <iostream>

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

    char readdata[FS_BLOCKSIZE];


    int status;
    status = fs_create("user18932738", "/first", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "//", 'f');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1", "/first", 'fd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create(
      "user1",
      "/firstidsfioiowejifeiuoffiwhfuhwheoiuhfuiwehfoiuwehiuohoiuefhoiufhoiuh",
      'fd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create(
      "user1", "/eudehuiodiehuiedsejefhiehihf83720947238094ehfieuoweohofwiodiweiuo",
      'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create(
      "user1", "/eudehuiodiehuiedsejefhiehihf83720947238094ehfieuoweoho", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1",
                       "/eudehuiodiehuiedsejefhiehihf83720947238094ehfieuoweoho/"
                       "eudehuiodiehuiedsejefhiehihf83720947238094ehfieuoweoho",
                       'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user1",
                       "/eudehuiodiehuiedsejefhiehihf83720947238094ehfieuoweoho/"
                       "eudehuiodiehuiedsejefhiehihf83720947238094ehfieuoweoho/"
                       "eudehuiodiehuiedsejefhiehihf83720947238094ehfieuoweoho",
                       'd');
    std::cout << "status of correct request " << status << std::endl;


    status = fs_create("user1", "/hi", 'd');
    std::cout << "status of correct request " << status << std::endl;

    status = fs_create("user2", "/hi/Low", 'd');
    std::cout << "status of correct request " << status << std::endl;
}