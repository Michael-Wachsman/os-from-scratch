#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <memory>
#include <vector>

using std::cout;
using std::endl;
using std::vector;


// thread *thrdptr = nullptr;
std::shared_ptr<thread> thread_ptr;

vector<thread> thread_vec;
bool starter = false;

void wait_on_fancy(void *a){
    auto my_ID = reinterpret_cast<long int>(a);
    cout << "Hi I'm " << my_ID << " I just started waiting" << endl;
    thread_ptr->join();
    cout << "Hi I'm " << my_ID << " I just started ended" << endl;
}

void fancy_locks(void *a){
    // auto my_ID = reinterpret_cast<long int>(a);
    // cout << my_ID << endl;
    // "
    thread::yield();

    // thread_ptr->join();

    // cout << my_ID << endl;
    cout << "doing some work everyone is waiting on after I yield" << endl;

}


void boot(void *in_n){
    
    // thrdptr = new thread(fancy_locks, reinterpret_cast<void *>(11));
    thread_ptr = std::make_shared<thread>(fancy_locks,reinterpret_cast<void *>(11));
    thread t1(wait_on_fancy, reinterpret_cast<void *>(1));
    thread t2(wait_on_fancy, reinterpret_cast<void *>(2));
    thread t3(wait_on_fancy, reinterpret_cast<void *>(3));
    thread t4(wait_on_fancy, reinterpret_cast<void *>(4));

}

int main(int argc, char *argv[]) {

  cpu::boot(1, boot, reinterpret_cast<void *>(445), false, false, 0);
  return 0;
}
