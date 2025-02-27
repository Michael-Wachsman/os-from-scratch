#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    char* shared = static_cast<char *>(vm_map(nullptr, 0));
	char* page1 = static_cast<char *>(vm_map(nullptr, 0));
	char* page2 = static_cast<char *>(vm_map(nullptr, 0));
	char* page3 = static_cast<char *>(vm_map(nullptr, 0));
	char* page4 = static_cast<char *>(vm_map(nullptr, 0));

	strcpy(page1, "page");
	strcpy(page2, "page");
	strcpy(shared, "This is shared data");
	// Evict page1
	strcpy(page3, "page");
	// Evict page2
	strcpy(page4, "page");

	// Now shared is at the front of the clock queue
	if (fork()) {
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