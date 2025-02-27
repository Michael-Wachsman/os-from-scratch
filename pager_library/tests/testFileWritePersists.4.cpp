#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
	auto filename = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(filename + 2, "lampson83.txt");
    char* page0 = static_cast<char *>(vm_map(filename + 2, 0));
	char* page1 = static_cast<char *>(vm_map(filename + 2, 0));
	char* page2 = static_cast<char *>(vm_map(filename + 2, 0));
	
	if (fork()) {
		cout << "Reading from page0" << endl;
		for (int i = 0; i < 20; ++i) {
			cout << page0[i];
		}
		cout << endl;

		auto word = "Blobs";
		for (size_t i = 0; i < strlen(word); ++i) {
			page0[i] = word[i];
		}
		cout << "Reading from page0 after write" << endl;
		for (int i = 0; i < 20; ++i) {
			cout << page0[i];
		}
		cout << endl;
	} else {
		cout << "Reading from page0" << endl;
		for (int i = 0; i < 20; ++i) {
			cout << page0[i];
		}
		cout << endl;

		cout << "Reading from page1" << endl;
		for (int i = 0; i < 20; ++i) {
			cout << page1[i];
		}
		cout << endl;
	}
}