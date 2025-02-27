#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
	if (fork()) {
		auto *page0 = static_cast<char *>(vm_map(nullptr, 0));
		auto *page1 = static_cast<char *>(vm_map(nullptr, 0));
		auto *page2 = static_cast<char *>(vm_map(nullptr, 0));
		page0[0] = 'a';
		page1[0] = 'a';
		page2[0] = 'a';
		page2[1] = 'c';
		for (int i = 0; i < 10; ++i) {
			cout << page0[i];
		}
		cout << endl;
		for (int i = 0; i < 10; ++i) {
			cout << page1[i];
		}
		cout << endl;
		for (int i = 0; i < 10; ++i) {
			cout << page2[i];
		}
		cout << endl;
	} else {
		auto *page0 = static_cast<char *>(vm_map(nullptr, 0));
		strcpy(page0, "Hello, world");
		for (int i = 0; i < 20; ++i) {
			cout << page0[i];
		}
		cout << endl;
	}
}