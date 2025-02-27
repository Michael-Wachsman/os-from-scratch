#include <iostream>

#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"

cv c;
mutex m;

void parent(void *a) {
  // This should throw a std::runtime_error
  try {
    c.wait(m);
  }
  catch (std::runtime_error &e) {
    std::cout << e.what() << std::endl;
  }
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}
