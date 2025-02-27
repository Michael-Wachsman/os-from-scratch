#include "../cpu.h"
#include <iostream>

using std::cout;
using std::endl;

int i = 0;
int j = 1;

void addI(void *_) {
  printf("Begin increment i\n");
  for (int count = 0; count < 1000; ++count) {
    i++;
  }
  printf("End increment i\n");
}

void multJ(void *_) {
  printf("Begin multincrement j\n");
  for (int count = 0; count < 20; ++count) {
    j *= 2;
  }
  printf("End multincrement j\n");
}

void parent(void *a) {
  thread t1(addI, nullptr);
  thread t2(multJ, nullptr);

  t1.join();
  t2.join();

  printf("%d %d\n", i, j);
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}