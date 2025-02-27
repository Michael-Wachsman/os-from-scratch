#include "../cpu.h"
#include <iostream>

using std::cout;
using std::endl;

void getReady(void *_) {
	printf("Get ready to join (and do nothing!)\n");
}

void parent(void *a) {
  thread t1(getReady, nullptr);
  thread::yield();
  printf("Thread 1 pre-join\n");
  t1.join();
  printf("Thread 1 post-join\n");
  t1.join();
}

int main() {
  cpu::boot(4, parent, reinterpret_cast<void *>(5), true, false, 1);
}