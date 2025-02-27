#include <cstring>
#include <iostream>

#include <stdio.h>
#include <unistd.h>

#include "vm_app.h"

using std::cout;

int main() {
    if (fork()) {
        char* filename = static_cast<char*>(vm_map(nullptr, 0));

        char* file = static_cast<char*>(vm_map(filename, 0));


        strcpy(file, "jeff");
    }
    else{
        char* filename = static_cast<char*>(vm_map(nullptr, 0));

        /* Write the name of the file that will be mapped */
        strcpy(filename, "lampson83.txt");

        strcpy(filename, "scrungle_bungus.txt");

        vm_yield();

        strcpy(filename, "jeffery.txt");

        vm_yield();
        printf("%s", filename);

        strcpy(filename, "Hello World I made it to the end");

        vm_yield();

        printf("%s", filename);
    }
}