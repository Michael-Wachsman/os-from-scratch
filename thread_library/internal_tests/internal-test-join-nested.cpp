#include "../cpu.h"
#include <iostream>
#include <memory>

using std::cout;
using std::endl;

int i = 0;
int j = 1;
int k = 0;

void subK(void *_) {
  printf("Start decrement k\n");
  for (int count = 0; count < 10; ++count) {
    k--;
  }
  printf("End decrement k\n");
}

void multJ(void *_) {
  thread t3(subK, nullptr);
  printf("Start multicrement j\n");
  for (int count = 0; count < 5; ++count) {
    j *= 2;
    if (count == 3) {
      t3.join();
    }
  }
  printf("End multicrement j = %d\n", j);
}

void addI(void *_) {
  thread t2(multJ, nullptr);
  printf("Thread 2 join\n");
  t2.join();

  printf("Start increment i\n");
  for (int count = 0; count < 1000; ++count) {
    i++;
  }
  printf("End increment i = %d\n", i);
}

void parent(void *a) {
  auto t1 = std::make_shared<thread>(addI, nullptr);
  printf("Thread 1 join\n");
  t1->join();
  printf("%d %d %d\n", i, j, k);
}

int main() {
  cpu::boot(4, parent, reinterpret_cast<void *>(5), true, false, 1);
}