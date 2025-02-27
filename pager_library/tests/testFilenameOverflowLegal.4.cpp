#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
	auto page0 = static_cast<char *>(vm_map(nullptr, 0));
	auto page1 = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(page1, "son83.txt");
	strcpy(page0, "bruh moment");
	auto filename = page0 + VM_PAGESIZE - 4;
	strncpy(filename, "lampson83.txt", 4);

	auto page2 = static_cast<char *>(vm_map(nullptr, 0));
	auto page3 = static_cast<char *>(vm_map(nullptr, 0));
	auto page4 = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(page2, "WE ARE SO BACK");
	// Evict page1
	strcpy(page3, "WE ARE SO DONE");
	// Evict page0
	strcpy(page4, "OH NO");

	auto page5 = static_cast<char *>(vm_map(filename, 0));
	// Evict page2
	for (int i = 0; i < 8; ++i) {
		cout << page5[i];
	}
	cout << endl;
	page5[0] = 'M';
	for (int i = 0; i < 8; ++i) {
		cout << page5[i];
	}
	cout << endl;
	cout << filename << endl;
}