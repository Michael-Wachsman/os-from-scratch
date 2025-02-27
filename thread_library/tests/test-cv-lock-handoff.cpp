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

  c.signal();
  c.wait(m);
 printf("Waiter %i is done waiting\n", arg);

  m.unlock();
}

void non_waiter(void *a) {
  auto arg = reinterpret_cast<intptr_t>(a);
  printf("Non_waiter %i awaken\n", arg);
  m.lock();
   printf("Non_waiter %i has taken the lock\n", arg);

  c.signal();
  m.unlock();
  printf("Non_waiter %i unlocks\n", arg);

}
void parent(void *_) {
  printf("Parent awaken\n");
  m.lock();
  thread(waiter, reinterpret_cast<void *>(1));
  thread(waiter, reinterpret_cast<void *>(2));
  thread(non_waiter, reinterpret_cast<void *>(3));
  thread(non_waiter, reinterpret_cast<void *>(4));
  thread::yield();
  m.unlock();
  printf("Parent dies\n");
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, false, false, 0);
}
