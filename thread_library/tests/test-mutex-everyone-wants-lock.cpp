#include "../cpu.h"
#include "../mutex.h"
#include "../thread.h"
#include <cstdio>

mutex m;

void lock_and_unlock(void *a) {
  auto arg = reinterpret_cast<intptr_t>(a);
  printf("Thread %ld tries to lock\n", arg);
  m.lock();
  if (arg == 2) {
    thread::yield();
  }
  printf("Thread %ld prints with the lock!\n", arg);
  if (arg == 1) {
    thread::yield();
  }
  m.unlock();
  printf("Thread %ld unlocks\n", arg);
}

void parent(void *arg) {
  thread t1(lock_and_unlock, reinterpret_cast<void *>(1));
  printf("Created thread 1\n");
  thread t2(lock_and_unlock, reinterpret_cast<void *>(2));
  printf("Created thread 2\n");
  thread::yield();
  thread t3(lock_and_unlock, reinterpret_cast<void *>(3));
  printf("Created thread 3\n");
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, true, false, 0);
  return 0;
}
