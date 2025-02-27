#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include <boost/thread.hpp>
#include <netinet/in.h>
#include <sys/socket.h>

#include "fs_param.h"

using std::string;
using std::vector;

// derived from the max_message size of each field
// FS_WRITEBLOCK = 13 chars
// 1 for each space, 3 for block number
// 1 for null byte of c string
const ssize_t MAX_HEADER_SIZE
  = 13 + 1 + FS_MAXUSERNAME + 1 + FS_MAXPATHNAME + 1 + 3 + 1;
int get_port_number(int sockfd);

int server_connect(int port);
int server_listen(int sockfd);

vector<string> split(char const* msg, char delimeter);
int handle_connection(int connectionfd);
