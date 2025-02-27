#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>

using std::cout;
using std::endl;

mutex m;
cv c;

void helloWorld(void *_) {
  m.lock();
  printf("Hello world!\n");
  m.unlock();
  thread::yield();
  m.lock();
  printf("Hello yield!\n");
  c.wait(m);
  thread::yield();
}

void goodbyeWorld(void *_) {
  printf("Goodbye world!\n");
  thread::yield();
  printf("Goodbye yield!\n");
  c.signal();
  thread::yield();
}

void parent(void *_) {
  {
    thread hello(helloWorld, nullptr);
    thread goodbye(goodbyeWorld, nullptr);
  }
  thread::yield();
  printf("Parent finished\n");
  thread::yield();
}

int main(int argc, char *argv[]) {
  cpu::boot(2, parent, nullptr, true, true, 1);
}
