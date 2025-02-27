#include "../cpu.h"
#include "../thread.h"
#include <iostream>

using std::cout;
using std::endl;

void helloWorld(void *_) {
  printf("Hello world!\n");
  thread::yield();
  printf("Hello yield!\n");
}

void parent(void *_) {
  { thread(helloWorld, nullptr); }
  thread::yield();
  printf("Parent finished\n");
  thread::yield();
}

int main(int argc, char *argv[]) {
  cpu::boot(4, parent, nullptr, true, false, 1);
}
