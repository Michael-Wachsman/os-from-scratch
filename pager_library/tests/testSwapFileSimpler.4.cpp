#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    /* Allocate swap-backed page from the arena */
    char* filename = static_cast<char *>(vm_map(nullptr, 0));

    /* Write the name of the file that will be mapped */
    strcpy(filename, "lampson83.txt");

    auto lamp_page = static_cast<char *>(vm_map(filename, 0));
    fork();
    strcpy(filename, "data1.bin");
    auto data_page = static_cast<char *>(vm_map(filename, 0));
    cout << lamp_page[0] << endl;
    cout << data_page[0] << endl;

    fork();
    cout << filename << endl;
    fork();

    strcpy(filename, "jeffery.txt");

    vm_yield();

    cout << filename << endl;
}