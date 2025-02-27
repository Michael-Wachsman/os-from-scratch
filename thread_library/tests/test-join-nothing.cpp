#include "../cpu.h"
#include <iostream>

using std::cout;
using std::endl;

void getReady(void *_) {
	cout << "Get ready to join (and do nothing!)" << endl;
}

void parent(void *a) {
  thread t1(getReady, nullptr);
  thread::yield();
  cout << "Thread 1 pre-join" << endl;
  t1.join();
  cout << "Thread 1 post-join" << endl;
  t1.join();
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}