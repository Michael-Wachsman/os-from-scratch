#include "../cpu.h"
#include "../mutex.h"
#include <iostream>

using std::cout;
using std::endl;

int i = 0;
int j = 1;
int k = 100;

mutex m;


void multJ(void *_) {
  printf("Start multicrement j\n");
  for (int count = 0; count < 10; ++count) {
    j *= 2;
  }
  printf("End multicrement j = %d\n", j);
}

void subI(void *_) {
  m.lock();
  printf("Start decrement i\n");
  for (int count = 0; count < 5; ++count) {
    --i;
  }
  printf("End decrement i\n");
  m.unlock();
}

void addI(void *_) {
  m.lock();
  thread t3(subI, nullptr);
  thread t4(subI, nullptr);
  thread t5(subI, nullptr);
  thread t6(subI, nullptr);
  thread t2(multJ, nullptr);
  printf("Thread 2 join\n");
  t2.join();

  printf("Start increment i\n");
  for (int count = 0; count < 1000; ++count) {
    i++;
  }
  printf("End increment i = %d\n", i);
  m.unlock();
}

void parent(void *a) {
  thread t1(addI, nullptr);
  printf("Thread 1 join\n");
  t1.join();
  printf("%d %d\n", i, j);
}

int main() {
  cpu::boot(4, parent, reinterpret_cast<void *>(5), true, true, 1);
}
