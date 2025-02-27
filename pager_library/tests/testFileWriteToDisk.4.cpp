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
	strncpy(page0, "Stuff", 5);
	auto page1 = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(page1, "Nobody expects the Spanish Inquisition!");
	auto page2 = static_cast<char *>(vm_map(nullptr, 0));
	// Should evict filename
	strcpy(page2, "I like turtles.");
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
	// Should evict page0, which is now dirty
	auto page3 = static_cast<char *>(vm_map(nullptr, 0));
	strcpy(page3, "WEEWOOWEEWOO");
	cout << "Reading from page0: " << endl;
	for (int i = 0; i < 16; ++i) {
		cout << page0[i];
	}
	cout << endl;

	cout << "Attempt to get another write fault on page0:" << endl;
	page0[0] = 'F';
	page0[1] = 'o';
	page0[2] = 'o';
	page0[3] = 'd';
	page0[4] = 's';

	cout << "Reading from page0: " << endl;
	for (int i = 0; i < 16; ++i) {
		cout << page0[i];
	}
	cout << endl;
}