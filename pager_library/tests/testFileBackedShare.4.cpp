#include <cstring>
#include <unistd.h>
#include <iostream>
#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
	auto *filename = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(filename, "lampson83.txt");
	if (fork()) {
		cout << "Entered fork if" << endl;
		auto *fb_page = static_cast<char *>(vm_map(filename, 0));
		fb_page[0] = 'B';
		cout << fb_page[0] << endl;
		vm_yield();
	} else {
		cout << "Entered not fork if" << endl;
		auto *fb_page = static_cast<char *>(vm_map(filename, 0));
		cout << "Is fb_page[0] 'B'?: " << (fb_page[0] == 'B') << endl;
		fb_page[0] = 'H';
		cout << fb_page[0] << endl;
	}
}