#include "../cpu.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <stdexcept>

mutex lock;

void child(void *arg) {
  try {
    printf("oro'\n");
    thread(child, reinterpret_cast<void *>(5));
    thread(child, reinterpret_cast<void *>(5));
    thread(child, reinterpret_cast<void *>(5));
    thread(child, reinterpret_cast<void *>(5));
    thread::yield();
    thread(child, reinterpret_cast<void *>(5));
  } catch (std::bad_alloc &e) {
    printf("%s\n", e.what());
    exit(0);
  }
}
void parent(void *arg) { thread(child, reinterpret_cast<void *>(5)); }

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
  return 0;
}
