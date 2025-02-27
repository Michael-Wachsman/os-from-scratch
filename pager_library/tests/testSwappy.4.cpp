#include <cstring>
#include <iostream>

#include <unistd.h>

#include "vm_app.h"


using std::cout;
using std::endl;

int main() {
    // map two swap-backed pages
    char* page0 = static_cast<char*>(vm_map(nullptr, 0));
    char* page1;

    for (int i = 0; i < 255; i++) {
        page1 = static_cast<char*>(vm_map(nullptr, 0));
    }

    // char *page1 = static_cast<char *>(vm_map(nullptr, 0));
    // strcpy(page1, "my name Bouser");

    for (int i = 0; i < 256; ++i) {
        strcpy(page0 + i * VM_PAGESIZE, "jeff");
    }
    cout << page0 << endl;
    vm_yield();
    cout << "egg" << endl;
}