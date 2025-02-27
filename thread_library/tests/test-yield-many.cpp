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
  cout << "Hello world!" << endl;
  m.unlock();
  thread::yield();
  m.lock();
  cout << "Hello yield!" << endl;
  c.wait(m);
  thread::yield();
}

void goodbyeWorld(void *_) {
  cout << "Goodbye world!" << endl;
  thread::yield();
  cout << "Goodbye yield!" << endl;
  c.signal();
  thread::yield();
}

void parent(void *_) {
  {
    thread hello(helloWorld, nullptr);
    thread goodbye(goodbyeWorld, nullptr);
  }
  thread::yield();
  cout << "Parent finished" << endl;
  thread::yield();
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, false, false, 0);
}
