#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../../thread.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;


mutex m1;
mutex m2;

vector<thread> thread_vec;


void fancy_locks(void *a){
    auto my_ID = reinterpret_cast<intptr_t>(a);

    m1.lock();


    cout << my_ID << endl;

    m2.lock();
    m1.unlock();

    cout << "-" << my_ID << endl;

    m2.unlock();
}

void funny_locks(void *a){
    auto my_ID = reinterpret_cast<intptr_t>(a);

    m2.lock();


    cout << "-" << my_ID <<  endl;

    m1.lock();
    m2.unlock();

    cout << my_ID << endl;

    m1.unlock();
}

void boot(void *in_n){

    for(int i = 0; i < 6; i++){
        if(i % 2 == 0){
            thread_vec.push_back( thread(fancy_locks, reinterpret_cast<void *>(i)));
        }
        else{
            thread_vec.push_back( thread(funny_locks, reinterpret_cast<void *>(i)));
        }
    }

    for(int i = 0; i < 6; i++){
        thread_vec[i].join();
    }

}

int main(int argc, char *argv[]) {

  cpu::boot(1, boot, reinterpret_cast<void *>(445), false, false, 0);
  return 0;
}