#include "networking.h"

#include <sstream>
#include <string>
#include <variant>

#include <unistd.h>

#include <sys/socket.h>

#include "filesystem.h"
#include "fs_param.h"
#include "parsing.h"

int get_port_number(int sockfd) {
    struct sockaddr_in addr;
    socklen_t length = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr*) &addr, &length) == -1) {
        perror("Error getting port of socket");
        return -1;
    }
    // Use ntohs to convert from network byte order to host byte order.
    return ntohs(addr.sin_port);
}

int make_server_sockaddr(struct sockaddr_in* addr, int port) {
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(port);
    return 0;
}


// just start ripping bgreeves example
int handle_connection(int connectionfd) {
    // 1 to handle overread before error
    char msg[MAX_HEADER_SIZE + 1];
    memset(msg, 0, sizeof(msg));

    // read until null byte
    ssize_t recvd = 0;
    ssize_t rval;
    do {
        rval = recv(connectionfd, msg + recvd, 1, 0);
        if (rval == -1) {
            perror("Error reading stream message");
            close(connectionfd);
            return -1;
        }
        recvd += rval;
        if (recvd > MAX_HEADER_SIZE) {
            // header too long, violated something
            close(connectionfd);
            return -1;
        }
    } while (rval > 0 && msg[recvd - 1]);

    printf("Client %d says '%s'\n", connectionfd, msg);

    auto maybe_parsed = parse_request(msg);
    if (!maybe_parsed.has_value()) {
        printf("Error\n");
    }
    else {
        auto parsed = maybe_parsed.value();
        printf("Parsed into: %s\n", request_string(parsed).c_str());

        if (std::holds_alternative<FS_WRITEBLOCK>(parsed)) {
            // if writeblock, read in another block of data
            FS_WRITEBLOCK& write_block = std::get<FS_WRITEBLOCK>(parsed);
            write_block.data = std::make_shared<std::array<char, FS_BLOCKSIZE>>();

            rval = recv(connectionfd, write_block.data->data(), FS_BLOCKSIZE,
                        MSG_WAITALL);
            if (rval == -1) {
                close(connectionfd);
                return -1;
            }
        }

        auto handled = execute_request(parsed);

        // check if error in handling
        if (handled.has_value()) {
            // send back correct response
            auto to_send = request_string(parsed);
            send(connectionfd, to_send.c_str(), to_send.size() + 1, MSG_NOSIGNAL);

            // send additional data block back if needed
            if (std::holds_alternative<FS_READBLOCK>(parsed)) {
                send(connectionfd, handled.value()->data(), FS_BLOCKSIZE,
                     MSG_NOSIGNAL);
            }
        }
    }


    // (4) Close connection
    close(connectionfd);

    return 0;
}

int server_connect(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    // (1) Create socket
    if (sockfd == -1) {
        perror("Error opening stream socket");
        return -1;
    }

    // (2) Set the "reuse port" socket option
    int yesval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof(yesval))
        == -1) {
        perror("Error setting socket options");
        return -1;
    }

    // (3) Create a sockaddr_in struct for the proper port and bind() to it.
    struct sockaddr_in addr;
    if (make_server_sockaddr(&addr, port) == -1) {
        return -1;
    }

    // (3b) Bind to the port.
    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        perror("Error binding stream socket");
        return -1;
    }

    return sockfd;
}

// bind to a socket and start listening
int server_listen(int sockfd) {
    listen(sockfd, -1);

    while (1) {
        int connectionfd = accept(sockfd, NULL, NULL);
        if (connectionfd == -1) {
            // might be unnessecary
            perror("Error accepting connection");
        }
        else {
            // spin off a thread
            boost::thread t(handle_connection, connectionfd);
            t.detach();
        }
    }
    return 0;
}
