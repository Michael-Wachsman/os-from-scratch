#include <cstring>
#include <iostream>

#include <unistd.h>

#include "vm_app.h"


using std::cout;
using std::endl;

int main() {
    // map two swap-backed pages
    vm_yield();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();
    fork();

    char* filename = static_cast<char*>(vm_map(nullptr, 0));

    vm_yield();


    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();
    // fork();


    // vm_yield();

    // fork();
    // fork();

    // vm_yield();

    // fork();

    // char* page0 = static_cast<char*>(vm_map(nullptr, 0));
}