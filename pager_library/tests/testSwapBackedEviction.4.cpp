#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include "vm_app.h"

using std::cout;

int main()
{
    //None of these should fault 
    char* sb_page0 = static_cast<char *>(vm_map(nullptr, 0));
    char* sb_page1 = static_cast<char *>(vm_map(nullptr, 0));
    char* sb_page2 = static_cast<char *>(vm_map(nullptr, 0));
    char* sb_page3 = static_cast<char *>(vm_map(nullptr, 0));


    // All of these should write fault
    strcpy(sb_page0, "I'm sb_page0");
    printf("%s\n", sb_page0);

    strcpy(sb_page1, "I'm sb_page1");
    printf("%s\n", sb_page1);

    strcpy(sb_page2, "I'm sb_page2");
    printf("%s\n", sb_page2);

    strcpy(sb_page3, "I'm sb_page3");
    printf("%s\n", sb_page3);
    //At this point sb_page0 should not be in PhysMem since PhysMem size is 4
    //The rest of the pages should be on memory

    strcpy(sb_page1, "I'm sb_page 1");
    printf("%s\n", sb_page1);
    strcpy(sb_page2, "I'm sb_page 2");
    printf("%s\n", sb_page2);
    strcpy(sb_page3, "I'm sb_page 3");
    printf("%s\n", sb_page3);
    // None of these should fault
    
    char* sb_page4 = static_cast<char *>(vm_map(nullptr, 0));

    //No fault should happen here
    printf("%s I'm page 4 and I am nullpage\n", sb_page4);

    //Write fault should happen
    strcpy(sb_page4, "I'm sb_page 4 and I am not the nullpage");
    printf("%s\n", sb_page4);
    //Now sb_page1 should have been evicted from PhysMem

    //No fault should happen here
    printf("%s\n ...again ", sb_page2);

}