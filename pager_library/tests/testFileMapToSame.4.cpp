#include <cstring>
#include <unistd.h>
#include <iostream>
#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
	char* filename = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(filename, "lampson83.txt");
	auto page0 = static_cast<char *>(vm_map(filename, 0));
	auto page1 = static_cast<char *>(vm_map(filename, 0));
	auto page2 = static_cast<char *>(vm_map(filename, 0));
	strcpy(filename, "data1.bin");
	auto data_page = static_cast<char *>(vm_map(filename, 0));
	cout << "Reading from page0: " << endl;
	for (int i = 0; i < 16; ++i) {
		cout << page0[i];
	}
	cout << endl;
	cout << "Reading from page1: " << endl;
	for (int i = 0; i < 16; ++i) {
		cout << page1[i];
	}
	cout << endl;
	cout << "Reading from page2: " << endl;
	for (int i = 0; i < 16; ++i) {
		cout << page2[i];
	}
	cout << endl;
	cout << "Reading from data_page" << endl;
	for (int i = 0; i < 32; ++i) {
		cout << data_page[i];
	}
	cout << endl;
}