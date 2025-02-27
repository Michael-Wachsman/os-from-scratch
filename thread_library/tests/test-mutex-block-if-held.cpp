#include "../cpu.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>

using std::cout;
using std::endl;

mutex m;

void child(void *_) {
  cout << "Child attempts to lock" << endl;
  m.lock();
  cout << "The lock must be free!" << endl;
  m.unlock();
}

void parent(void *arg) {
  thread t1(child, nullptr);
  cout << "Parent locked" << endl;
  m.lock();
  thread::yield();
  m.unlock();
  cout << "Parent unlocked" << endl;
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, false, false, 0);
  return 0;
}
