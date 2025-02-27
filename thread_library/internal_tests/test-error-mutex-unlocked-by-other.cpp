#include <iostream>
#include <stdexcept>

#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"

mutex m;

void child(void *_) {
  printf("Child attempts to unlock parent's lock\n");
  try {
    m.unlock();
  } catch (std::runtime_error &e) {
    printf("%s\n", e.what());
  }
  printf("Child unlocks but shouldn't have\n");
}

void parent(void *a) {
  thread(child, nullptr);
  m.lock();
  printf("Parent locks\n");
  thread::yield();
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}
