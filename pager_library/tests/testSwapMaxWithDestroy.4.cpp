#include <iostream>

#include <unistd.h>

#include "vm_app.h"

using std::cout;

int main(int argc, char* argv[]) {
    // char* first = reinterpret_cast<char*>(vm_map(nullptr, 24));
    if (fork()) {
        int num_pages = 0;

        while (true) {
            char* next_swap = reinterpret_cast<char*>(vm_map(nullptr, 24));
            if (next_swap) {
                // if (num_pages == 20) {
                //     // fork();
                //     vm_yield();
                // }
                num_pages += 1;
                *next_swap = 'a';
            } else {
                break;
            }
        }
        cout << "made " << num_pages << " before failing " << std::endl;

    } else {
        int num_pages = 0;
        while (true) {
            char* next_swap = reinterpret_cast<char*>(vm_map(nullptr, 24));
            if (next_swap) {
                num_pages += 1;
                *next_swap = 'a';
            } else {
                break;
            }
        }
        cout << "made " << num_pages << " before failing " << std::endl;
    }

    return 0;
}
