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
		strncpy(lamp_page, "Attempts to", 11);
	} else {
		for (int i = 0; i < 15; ++i) {
			cout << lamp_page[i];
			if (i == 10) {
				vm_yield();
			}
		}
		cout << endl;
	}
}