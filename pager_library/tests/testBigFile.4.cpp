#include <cstring>
#include <iostream>

#include <unistd.h>

#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
    if (fork()) {
        /* Allocate swap-backed page from the arena */
        char* filename = static_cast<char*>(vm_map(nullptr, 0));

        /* Write the name of the file that will be mapped */
        strcpy(filename, "data1.bin");

        /* Map a page from the specified file */
        char* p = static_cast<char*>(vm_map(filename, 1));
        //strcpy(p, "I'm mr J");
        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 30; i++) {
            cout << p[i];
        }
        cout << endl;
    } else {
        char* filename = static_cast<char*>(vm_map(nullptr, 0));

        /* Write the name of the file that will be mapped */
        strcpy(filename, "data1.bin");

        /* Map a page from the specified file */
        char* p = static_cast<char*>(vm_map(filename, 0));

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 30; i++) {
            cout << p[i];
        }
        cout << endl;
    }
}