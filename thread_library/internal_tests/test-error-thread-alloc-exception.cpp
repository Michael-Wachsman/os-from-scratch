#include "cpu.h"
#include "thread.h"
#include <iostream>
#include <new>

void propogate(void *_) {}

void parent(void *_) {
  while (true) {
    try {
      // keeps creating thread memory and discarding handle
      thread(propogate, nullptr);
    } catch (std::bad_alloc &e) {
      std::cout << "caught a bad alloc error:" << e.what() << std::endl;
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, false, false, 0);
  return 0;
}
