#include "../cpu.h"
#include "../cv.h"
#include "../thread.h"
#include <iostream>

cv c;

void parent(void *a) {
  printf("before signal\n");
  c.signal();
  printf("after signal\n");
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}
