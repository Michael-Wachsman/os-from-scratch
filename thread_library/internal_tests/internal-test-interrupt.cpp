#include "../cpu.h"
#include <iostream>

using std::cout;
using std::endl;

int other_count = 0;

void another(void *_) {
  for (int i = 0; i < 100; ++i) {
    printf("another %d\n", i);
  }
}

void other(void *arg) {
  ++other_count;
  printf("other called\n");
}

void parent(void *arg) {
  thread t1(other, nullptr);
  thread t2(another, nullptr);
  printf("Enter while loop\n");
  while (1) {
  };
  printf("I got out?\n");
}

int main(int argc, char *argv[]) {
  cpu::boot(2, parent, nullptr, true, false, 1);
}
