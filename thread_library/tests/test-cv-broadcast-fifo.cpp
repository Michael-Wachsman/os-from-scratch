#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>

using std::cout;
using std::endl;

mutex m;
cv c;

void waiter(void *a) {
  auto arg = reinterpret_cast<intptr_t>(a);
  printf("Waiter %i awaken\n", arg);
  m.lock();
  printf("Waiter %i takes the lock\n", arg);

  c.wait(m);
  printf("Waiter %i is done waiting\n", arg);

  m.unlock();
}

void parent(void *_) {
  printf("Parent awaken\n");
  m.lock();
  thread(waiter, reinterpret_cast<void *>(1));
  thread(waiter, reinterpret_cast<void *>(2));
  thread(waiter, reinterpret_cast<void *>(3));
  thread(waiter, reinterpret_cast<void *>(4));
  m.unlock();
  thread::yield();
  c.broadcast();
  printf("Parent dies\n");
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, false, false, 0);
}
