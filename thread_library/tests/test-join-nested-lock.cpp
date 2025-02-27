#include "../cpu.h"
#include "../mutex.h"
#include <iostream>

using std::cout;
using std::endl;

int i = 0;
int j = 1;
int k = 100;

mutex m;


void multJ(void *_) {
  cout << "Start multicrement j" << endl;
  for (int count = 0; count < 10; ++count) {
    j *= 2;
  }
  cout << "End multicrement j = " << j << endl;
}

void subI(void *_) {
  m.lock();
  cout << "Start decrement i" << endl;
  for (int count = 0; count < 5; ++count) {
    --i;
  }
  cout << "End decrement i" << endl;
  m.unlock();
}

void addI(void *_) {
  m.lock();
  thread t3(subI, nullptr);
  thread t4(subI, nullptr);
  thread t5(subI, nullptr);
  thread t6(subI, nullptr);
  thread t2(multJ, nullptr);
  cout << "Thread 2 join" << endl;
  t2.join();

  cout << "Start increment i" << endl;
  for (int count = 0; count < 1000; ++count) {
    i++;
  }
  cout << "End increment i = " << i << endl;
  m.unlock();
}

void parent(void *a) {
  thread t1(addI, nullptr);
  cout << "Thread 1 join" << endl;
  t1.join();
  cout << i << " " << j << endl;
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}
