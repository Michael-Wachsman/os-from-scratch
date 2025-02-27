#include "../cpu.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>

mutex lock;

void parent(void *arg) {
  std::cout << "I started" << std::endl;
  lock.lock();
  std::cout << "I locked" << std::endl;
  lock.lock(); // should deadlock
  lock.unlock();
  // should NOT print
  std::cout << "I unlocked?" << std::endl;
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, false, false, 0);
  return 0;
}
