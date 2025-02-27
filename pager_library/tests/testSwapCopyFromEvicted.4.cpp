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

	strcpy(page1, "I AM PAGE 1");
	strcpy(page2, "GORILLA TAPE");
	strcpy(shared, "This is shared data");
	// Evict page1
	strcpy(page3, "page");
	// Evict page2
	strcpy(page4, "GORILLA GLUE");

	// Now shared is at the front of the clock queue
	if (fork()) {
		strcpy(page4, page2);
		cout << "Fork: " << page4 << endl; // "GORILLA TAPE"
	} else {
		cout << "Nonfork: " << page4 << endl; // "GORILLA GLUE"
	}
}