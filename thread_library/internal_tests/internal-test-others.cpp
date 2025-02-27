#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::vector;

std::shared_ptr<thread> thread_ptr;
vector<thread> thread_vec;
bool starter = false;

void wait_on_fancy(void *a) {
  auto my_ID = reinterpret_cast<long int>(a);
  printf("Hi I'm %ld I just started waiting\n", my_ID);
  printf("Hi I'm %ld I just ended\n", my_ID);
}

void fancy_locks(void *a) {
  thread::yield();
  printf("doing some work everyone is waiting on after I yield\n");
}

void boot(void *in_n) {
  thread_ptr =
      std::make_shared<thread>(fancy_locks, reinterpret_cast<void *>(11));
  thread t1(wait_on_fancy, reinterpret_cast<void *>(1));
  thread t2(wait_on_fancy, reinterpret_cast<void *>(2));
  thread t3(wait_on_fancy, reinterpret_cast<void *>(3));
  thread t4(wait_on_fancy, reinterpret_cast<void *>(4));
}

int main(int argc, char *argv[]) {
  cpu::boot(2, boot, reinterpret_cast<void *>(445), true, false, 1);
  return 0;
}
