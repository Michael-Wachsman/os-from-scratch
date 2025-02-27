#include <cstring>
#include <iostream>
#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
	char* filename = static_cast<char *>(vm_map(nullptr, 0));

    strcpy(filename, "lampson83.txt");

    /* Map a page from the specified file */
    char* page0 = static_cast<char *>(vm_map (nullptr, 0));
	page0[0] = 'a';
	char* page1 = static_cast<char *>(vm_map (nullptr, 0));
	page1[0] = 'b';
	char* page2 = static_cast<char *>(vm_map (nullptr, 0));
	page2[0] = 'c';

	// filename should be evicted by now
	// This will evict page0
	strcpy(filename + 13, "Bringing this page back from memory");
	cout << filename << endl;

	cout << page0 << endl;
}