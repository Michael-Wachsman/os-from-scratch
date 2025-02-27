#include "../cpu.h"
#include "../thread.h"
#include <iostream>

using std::cout;
using std::endl;

void helloWorld(void *_) {
  cout << "Hello world!" << endl;
  thread::yield();
  cout << "Hello yield!" << endl;
}

void parent(void *_) {
  { thread(helloWorld, nullptr); }
  thread::yield();
  cout << "Parent finished" << endl;
  thread::yield();
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, false, false, 0);
}
