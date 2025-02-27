#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include "vm_app.h"

using std::cout;

int main()
{

    if(fork()){
        char* page_a = static_cast<char *>(vm_map(nullptr, 0));
        strcpy(page_a, "I am page a");
        printf("%s\n", page_a);
        vm_yield();

        char* page_b = static_cast<char *>(vm_map(nullptr, 3));
        strcpy(page_b, "I am page b");
        printf("%s\n", page_b);
        vm_yield();


        char* page_c = static_cast<char *>(vm_map(nullptr, 6));
        strcpy(page_c, "I am page c and I evicted someone");
        printf("%s\n", page_c);
        //This should evict something
        vm_yield();

        printf("%s, I just got back in memory\n", page_a);
        //read fault and eviction should happen 

        //Now this process destroys itself

    }
    else{
        char* page_0 = static_cast<char *>(vm_map(nullptr, 0));
        strcpy(page_0, "I am page 0");
        printf("%s\n", page_0);
        vm_yield();

        char* page_1 = static_cast<char *>(vm_map(nullptr, 0));
        strcpy(page_1, "I am page 1");
        printf("%s\n", page_1);
        //memory should be full
        vm_yield();
        

        char* page_2 = static_cast<char *>(vm_map(nullptr, 0));
        strcpy(page_2, "I am page 2 and I evicted someone");
        printf("%s\n", page_2);
        //this should evict something
        
        printf("%s, I am also in Memory\n", page_1);
        //no faults or evictions should happen 
        vm_yield();


        strcpy(page_0, "I am page 0 and I didn't evict someone");
        printf("%s\n", page_0);
        // this should write fault but not evict
    }
}