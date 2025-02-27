#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    char* shared = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(shared, "This is shared data");

	if (fork()) {
		cout << "Fork: " << shared << endl; // "This is shared data"
		auto word = "banana";
		// Don't want to write the null terminator
		for (int i = 0; i < 6; ++i) {
			shared[i + 8] = word[i];
		}
		cout << "Fork: " << shared << endl; // "This is banana data"
	} else {
		cout << "Nonfork: " << shared << endl; // "This is shared data"
	}
}