#include "../cpu.h"
#include "../mutex.h"
#include "../thread.h"
#include <cstdio>

mutex lock;

void parent(void *arg) {
  printf("I started\n");
  lock.lock();
  printf("I locked\n");
  lock.lock(); // should deadlock
  lock.unlock();
  // should NOT print
  printf("I a unlocked\n");
}

int main(int argc, char *argv[]) {
  cpu::boot(2, parent, nullptr, true, false, 1);
  return 0;
}
