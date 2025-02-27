// I need to produce incorrect output with just swap files, and no forks
// we know there's also a failure after the filename is ""

#include <cstring>
#include <iostream>

#include <unistd.h>

#include "vm_app.h"


using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    auto p1 = static_cast<char*>(vm_map(nullptr, 0));
    auto p2 = static_cast<char*>(vm_map(nullptr, 10));
    p2[0] = 'a';

    // copy from one nullpage to another
    p1[(int) *p2] = *p2;
    *p1 += 'b';
    p2[(int) *p1] = *p1;
    ++*p1;
    auto p3 = static_cast<char*>(vm_map(nullptr, 0));
    auto p4 = static_cast<char*>(vm_map(nullptr, 10));

    cout << p1['a'] << endl;
    cout << *p1 << endl;
    cout << p2[*p1] << endl;

    // if (fork()) {
    //     auto p1 = static_cast<char*>(vm_map(nullptr, 0));
    //     p1[10] = 'a';
    //
    //     auto p2 = static_cast<char*>(vm_map(nullptr, 0));
    //     *p2 = 'b';
    //     auto p3 = static_cast<char*>(vm_map(nullptr, 0));
    //     *p3 = 'c';
    //
    //     vm_yield();
    //
    //     cout << p1[10] << endl;

    //     auto p4 = static_cast<char*>(vm_map(nullptr, 0));
    //     *p4 = 'd';
    //
    //     auto p5 = static_cast<char*>(vm_map(nullptr, 0));
    //     *p5 = 'e';
    //
    //     // auto p6 = static_cast<char*>(vm_map(nullptr, 0));
    //     // *p6 = 'f';
    // }


    return 0;
}
