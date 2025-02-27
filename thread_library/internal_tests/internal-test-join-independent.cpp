#include "../cpu.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::to_string;

int i = 0;
int j = 0;

void addI(void *_) {
  printf("Begin increment i = %d\n", i++);
  for (int count = 0; count < 1000; ++count) {
    i++;
  }
  printf("End increment i\n");
}

void multJ(void *_) {
  printf("Begin multicrement j = %d\n", j++);
  for (int count = 0; count < 10; ++count) {
    j *= 2;
  }
  printf("End multicrement j\n");
}

void parent(void *a) {
  thread t1(addI, nullptr);
  thread t2(multJ, nullptr);

  printf("t1 join()\n");
  t1.join();
  printf("t2 join()\n");
  t2.join();

  printf("%d %d\n", i, j);
}

int main() {
  cpu::boot(4, parent, reinterpret_cast<void *>(5), true, false, 1);
}