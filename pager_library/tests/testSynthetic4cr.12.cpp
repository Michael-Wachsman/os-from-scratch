#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include <stdio.h>
#include <unistd.h>

#include "vm_app.h"

using std::cout;
//

int main(int argc, char* argv[]) {
    // make 2^num_forks processes, empty arena
    int num_forks = 5;

    int seed = 0;
    for (int i = 0; i < num_forks; i++) {
        seed += fork();
    }

    std::vector<char*> swap_maps;
    std::vector<char*> file_maps;
    char* filename = static_cast<char*>(vm_map(nullptr, 0));
    assert(filename);
    strcpy(filename, "lampson83.txt");


    // forks all have same behavior, could change this later

    int i = seed;
    while (true) {
        if (i % 1000 == 500) {
            exit(0);
        }

        // 10 % yields
        if (i % 10 == 1) {
            vm_yield();
        }

        // 20 % make a new swap backed page
        if (i % 5 == 0) {
            char* new_swap = reinterpret_cast<char*>(vm_map(nullptr, 0));
            if (new_swap) {
                int zero = new_swap[0];
                // cout << "swap map size: " << swap_maps.size() << std::endl;
                swap_maps.push_back(new_swap);
                assert(!zero);
            }
        }

        // 20% write to all swap backed pages
        if (i % 5 == 1) {
            // for (auto ptr : swap_maps) {
            auto ptr = swap_maps[(17 * i) % swap_maps.size()];
            ptr[2 * i % 37] = 'g';
            // }
            if (!swap_maps.empty()) {
                assert(swap_maps[(17 * i) % swap_maps.size()][2 * i % 37] == 'g');
            }
        }

        // 20% make a file map
        if (i % 5 == 2) {
            // cout << "entering file mapping" << std::endl;
            char* new_file_map = reinterpret_cast<char*>(vm_map(filename, 0));
            if (new_file_map) {
                file_maps.push_back(new_file_map);
            }
            // assert(new_file_map);
            // cout << "File map size: " << file_maps.size() << std::endl;
        }
        //
        // write to all file maps
        if (i % 5 == 3) {
            // cout << "entering file writing" << std::endl;
            // for (auto ptr : file_maps) {
            auto ptr = file_maps[(i * 17) % file_maps.size()];
            strcpy(ptr + ((3 * i) % 527), "words of hope");
            // }
            if (!file_maps.empty()) {
                assert(!strcmp(file_maps[(i * 17) % file_maps.size()] + ((3 * i) % 527),
                               "words of hope"));
            }
        }
        i++;
    }

    return 0;
}
