#include <cstdio>

#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"

cv c;

mutex m1;
mutex m2;
mutex m3;

void parent(void *a) {
  m1.lock();
  m2.lock();
  m3.lock();
  c.wait(m1);
  c.signal();
  c.wait(m2);
  c.signal();
  c.wait(m3);
  c.broadcast();
  printf("End of bad cv usage\n");
}

int main() {
  cpu::boot(2, parent, reinterpret_cast<void *>(5), true, false, 1);
}
