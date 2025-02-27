#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
	auto page0 = static_cast<char *>(vm_map(nullptr, 0));
	auto filename = page0 + VM_PAGESIZE - 4;
	strcpy(filename, "lampson83.txt");

	auto page2 = static_cast<char *>(vm_map(filename, 0));
	// Should fail to read from "lamp"?
	for (int i = 0; i < 8; ++i) {
		cout << page2;
	}
	cout << endl;
}