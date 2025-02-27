#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include <stdio.h>
#include <unistd.h>

#include "vm_app.h"
#include "vm_arena.h"

using std::cout;
//

int main(int argc, char* argv[]) {
    // make 2^num_forks processes, empty arena
    int max_forks = 5;

    int seed = 0;
    // for (int i = 0; i < num_forks; i++) {
    //     seed += fork();
    // }

    std::vector<char*> swap_maps;
    std::vector<char*> file_maps;
    char* filename = static_cast<char*>(vm_map(nullptr, 0));
    assert(filename);
    strcpy(filename, "data2.bin");

    char* lamp = static_cast<char*>(vm_map(nullptr, 0));
    assert(filename);
    strcpy(lamp, "lampson83.txt");

    char* bad_filename = static_cast<char*>(vm_map(nullptr, 0));
    assert(bad_filename);
    strcpy(bad_filename, "nope.txt");

    char* fork_file = static_cast<char*>(vm_map(nullptr, 0));
    strcpy(fork_file, "data1.bin");
    int* num_forks = static_cast<int*>(vm_map(fork_file, 0));

    *num_forks = 0;


    // NEED TO ADD MORE ERROR CASES
    // MORE RANDOM READS, GENUINELY RANDOM READS


    // forks all have same behavior, could change this later

    int i = seed;
    while (true) {
        if (i % 20388 == 500) {
            exit(0);
        }

        // i % 27 == 0 and *num_forks < max_forks
        if (i % 10 == 0) {
            *num_forks += 1;
            fork();
            cout << "forked !" << std::endl;
        }

        if (i % 59 == 58) {
            // invalid file map
            char* new_file_map = reinterpret_cast<char*>(vm_map(bad_filename, 0));
            if (new_file_map) {
                file_maps.push_back(new_file_map);
            }
        }
        if (i % 59 == 51) {
            // valid map to invalid block
            char* new_file_map = reinterpret_cast<char*>(vm_map(lamp, i));
            if (new_file_map) {
                file_maps.push_back(new_file_map);
            }
        }


        // 10 % yields
        if (i % 10 == 1) {
            vm_yield();
        }

        // do some spicier bounds issues
        if (i % 30 == 8) {
            if (!swap_maps.empty()) {
                auto data = swap_maps[(i * 19) % swap_maps.size()]
                          + ((4 * i % 6) * (VM_PAGESIZE / 3))
                          - (3 * i % 5) * (VM_PAGESIZE / 4);
            }
        }

        if (i % 30 == 4) {
            if (!file_maps.empty()) {
                auto data = file_maps[(i * 42) % file_maps.size()]
                          + ((7 * i % 9) * (VM_PAGESIZE / 3))
                          - (3 * i % 5) * (VM_PAGESIZE / 4);
            }
        }

        // 20% make a new swap backed page
        if (i % 5 == 0) {
            char* new_swap = reinterpret_cast<char*>(vm_map(nullptr, 0));
            if (new_swap) {
                //     int zero = new_swap[0];
                //     // cout << "swap map size: " << swap_maps.size() << std::endl;
                swap_maps.push_back(new_swap);
                //     assert(!zero);
            }
        }

        // 20% write to a few of their swap backed pages
        if (i % 5 == 1) {
            for (int j = 0; j < (i % 9); j++) {
                if (!swap_maps.empty()) {
                    auto ptr = swap_maps[(17 * (i + j)) % swap_maps.size()];
                    ptr[2 * (i + j) % 37] = 'g';
                }
                // }
                // if (!swap_maps.empty()) {
                //     assert(swap_maps[(17 * i) % swap_maps.size()][2 * i % 37] ==
                //     'g');
                // }
            }
        }

        // 10% make a file map
        if (i % 10 == 2) {
            // cout << "entering file mapping" << std::endl;
            char* new_file_map = reinterpret_cast<char*>(vm_map(filename, i % 37));
            if (new_file_map) {
                file_maps.push_back(new_file_map);
            }
            // assert(new_file_map);
            // cout << "File map size: " << file_maps.size() << std::endl;
        }
        //
        // write to many file maps
        if (i % 4 == 3) {
            // cout << "entering file writing" << std::endl;
            for (int j = 0; j < (i % 7); j++) {
                if (!file_maps.empty()) {
                    auto ptr = file_maps[((i + j) * 17) % file_maps.size()];
                    strcpy(ptr + ((3 * (i + j)) % 527), "words of hope");
                }
            }
            // }
            // if (!file_maps.empty()) {
            //     assert(!strcmp(file_maps[(i * 17) % file_maps.size()] + ((3 * i) %
            //     527),
            //                    "words of hope"));
            // }
        }
        i++;
    }

    return 0;
}
