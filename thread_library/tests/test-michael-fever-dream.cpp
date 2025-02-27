#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

cv cv1;

mutex m1;
mutex m2;

std::shared_ptr<thread> thread_ptr;

bool starter = false;
bool all_clear = false;

void fancy_locks(void *a) {
  auto my_ID = reinterpret_cast<intptr_t>(a);

  cout << "Fancy: " << my_ID << endl;

  thread_ptr->join();

  cout << "Fancy: " << my_ID << " is free, now to wait." << endl;
  m2.lock();
  while (!all_clear) {
    cv1.wait(m2);
  }
  cout << "Fancy: " << my_ID << " is now done waiting." << endl;
  m2.unlock();
  cv1.signal();
}

void funny_lock(void *a) {
  m1.lock();
  cout << "Hi guys, Funny is about to wait till my join queue is full" << endl;
  while (!starter) {
    cv1.wait(m1);
  }
  cout << "Funny join queue is full, time to empty" << endl;
  m1.unlock();
}

void funky_lock(void *a) {
  auto my_ID = reinterpret_cast<intptr_t>(a);

  cout << "Funky: " << my_ID << endl;

  thread_ptr->join();

  cout << "Funky is about to free all fancy " << endl;
  all_clear = true;
  cv1.signal();
}

void boot(void *in_n) {
  cout << "Hi guys, I just booted" << endl;
  thread_ptr =
      std::make_shared<thread>(funny_lock, reinterpret_cast<void *>(10));
  thread(fancy_locks, reinterpret_cast<void *>(1));
  thread(fancy_locks, reinterpret_cast<void *>(2));
  thread(fancy_locks, reinterpret_cast<void *>(3));
  thread(funky_lock, reinterpret_cast<void *>(1));
  thread(fancy_locks, reinterpret_cast<void *>(5));
  thread(fancy_locks, reinterpret_cast<void *>(6));

  cout << "About to free funny lock" << endl;
  thread::yield();
  starter = true;
  cv1.signal();
  cout << "Just freed funny lock" << endl;
}

int main(int argc, char *argv[]) {

  cpu::boot(1, boot, reinterpret_cast<void *>(445), false, false, 0);
  return 0;
}