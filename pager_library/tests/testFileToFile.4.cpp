#include <cstring>
#include <unistd.h>
#include <iostream>
#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
	char* filename = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(filename, "lampson83.txt");
	auto metafilename = static_cast<char *>(vm_map(filename, 0));
	strcpy(metafilename, "data1.bin");
	auto page2 = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(page2, "P");
	auto page3 = static_cast<char *>(vm_map(nullptr, 0));
	// Evict filename
	strcpy(page3, "P");
	auto page4 = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(page4, "P");
	auto filenametwo = static_cast<char *>(vm_map(filename, 0));
	cout << filenametwo << endl;
	filenametwo += 6;
	for (int i = 0; i < 12; ++i) {
		cout << filenametwo[i + 6];
	}
	cout << endl;
}