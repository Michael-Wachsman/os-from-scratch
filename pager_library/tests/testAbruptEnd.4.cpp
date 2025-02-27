#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
	auto filename = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(filename, "lampson83.txt");
	auto lamp_page = static_cast<char *>(vm_map(filename, 0));
	if (fork()) {
		strncpy(lamp_page, "Bread", 5);
		int* i = nullptr;
		cout << *i << endl;
		strncpy(lamp_page, "Plane", 5);
	} else {
		for (int i = 0; i < 16; ++i) {
			cout << lamp_page[i];
		}
		cout << endl;
	}
}