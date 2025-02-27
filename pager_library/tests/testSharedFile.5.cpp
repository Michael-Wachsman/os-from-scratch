#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;

int main()
{
    /* Allocate swap-backed page from the arena */
    char* filename = static_cast<char *>(vm_map(nullptr, 0));

    /* Write the name of the file that will be mapped */

    strcpy(filename, "Junktown.txt");

    fork();
    fork();
    fork();

    printf("%s", filename);

    
    strcpy(filename, "scrungle_bungus.txt");

    vm_yield();

    strcpy(filename, "jeffery.txt");

    vm_yield();
    printf("%s", filename);

    strcpy(filename, "Hello World I made it to the end");

    printf("%s", filename);
}
