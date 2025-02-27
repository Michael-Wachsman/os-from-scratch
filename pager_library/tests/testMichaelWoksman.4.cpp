#include <cstring>
#include <iostream>
#include <vector>

#include <unistd.h>

#include "vm_app.h"

using std::cout;

int main() {
    int j = 0;
    if (fork()) {
        if (fork()) {
            j = 1637;
        } else {
            if (fork()) {
                j = 3785;
            } else {
                j = 552;
            }
        }
    } else {
        j = 4770;
    }

    std::vector<char*> swap_maps;

    while (true) {
        if (j % 3000 == 89) {
            break;
        }

        if (j % 70 == 22) {
            char* new_swap = reinterpret_cast<char*>(vm_map(nullptr, j));
            if (new_swap) {
                swap_maps.push_back(new_swap);
            }
        }

        if (j % 10 == 0) {
            vm_yield();
        }

        if (j % 7 == 2) {
            if (!swap_maps.empty()) {
                char herald = *(swap_maps[(j * 17) % swap_maps.size()]);
            }
        }

        if (j % 11 == 4) {
            if (!swap_maps.empty()) {
                strcpy(swap_maps[(j * 12) % swap_maps.size()], "k");
            }
        }


        j++;
    }
}