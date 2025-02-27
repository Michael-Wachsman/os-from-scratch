#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <memory>
#include <vector>

using std::vector;

// thread *thrdptr = nullptr;
std::shared_ptr<thread> thread_ptr;

vector<thread> thread_vec;
bool starter = false;

void fancy_locks(void *a) {
  auto my_ID = reinterpret_cast<long int>(a);
  printf("%ld: I started\n", my_ID);
  thread_ptr->join();
  printf("%ld: I joined with myself!\n", my_ID);
}

void boot(void *in_n) {

  // thrdptr = new thread(fancy_locks, reinterpret_cast<void *>(11));
  thread_ptr =
      std::make_shared<thread>(fancy_locks, reinterpret_cast<void *>(11));

  starter = true;

  // delete thrdptr;
}

int main(int argc, char *argv[]) {
  cpu::boot(4, boot, reinterpret_cast<void *>(445), true, false, 1);
  return 0;
}
