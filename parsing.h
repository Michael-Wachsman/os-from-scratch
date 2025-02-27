
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "request.h"
using std::string;
using std::vector;
using std::optional;

vector<string> split(char const* msg, char delimeter);

optional<Request> parse_request(char const* msg);
