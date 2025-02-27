#include "fs_server.h"

#include <iostream>
#include <ostream>

#include "filesystem.h"
#include "networking.h"

int main(int argc, char* argv[]) {
    int port;
    if (argc != 2) {
        // OS chooses port if it's 0
        port = 0;
    }
    else {
        port = atoi(argv[1]);
    }

    int sockfd = server_connect(port);
    if (sockfd == -1) {
        return -1;
    }

    port = get_port_number(sockfd);

    fs_init();
    print_port(port);

    server_listen(sockfd);
    return 0;
}
