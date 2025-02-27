#include <cstring>
#include <iostream>

#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>

#include "vm_app.h"

using std::cout;
using std::endl;

int main() {
    for (int i = 0; i < 10; i++) {
        if (fork()) {
            if (fork()) {
                char* page_a = static_cast<char*>(vm_map(nullptr, 0));
                char* page_b = static_cast<char*>(vm_map(nullptr, 0));

                if (i % 3 == 0) {
                    strcpy(page_b, "lampson83.txt");
                    char* page_c = static_cast<char*>(vm_map(page_b, 0));
                    if (i % 7 != 0) {
                        vm_yield();
                        char *blob = reinterpret_cast<char *>(i);
                        strcpy(page_c, blob);
                    }
                    auto b = page_c[i];

                }

                strcpy(page_a, "I am page a");


                // Now this process destroys itself
            } else {
                if (i % 2 == 0) {
                    continue;
                } else {
                    char* page_k = static_cast<char*>(vm_map(nullptr, 0));
                    char* page_s = static_cast<char*>(vm_map(nullptr, 0));
                    strcpy(page_k, "data2.bin");
                    vm_yield();
                    char* page_c = static_cast<char*>(vm_map(page_k, i));
                    auto b = page_c[i];
                }
            }

        } else {
            char* page_a = static_cast<char*>(vm_map(nullptr, 0));
            char* page_b = static_cast<char*>(vm_map(nullptr, 0));
            strcpy(page_b, "lampson83.txt");
            char* page_c = static_cast<char*>(vm_map(page_b, 0));

            char* page_0 = static_cast<char*>(vm_map(nullptr, 0));
            strcpy(page_0, "I am page 0");
            printf("%s\n", page_0);
            vm_yield();

            char* page_1 = static_cast<char*>(vm_map(nullptr, 0));
            strcpy(page_1, "I am page 1");
            printf("%s\n", page_1);
            // memory should be full
            vm_yield();


            char* page_2 = static_cast<char*>(vm_map(nullptr, 0));
            strcpy(page_2, "I am page 2 and I evicted someone");
            printf("%s\n", page_2);
            // this should evict something

            printf("%s, I am also in Memory\n", page_1);
            // no faults or evictions should happen
            vm_yield();


            strcpy(page_0, "I am page 0 and I didn't evict someone");
            printf("%s\n", page_0);
            // this should write fault but not evict
        }
    }
}