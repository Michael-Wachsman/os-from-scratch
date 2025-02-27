#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

vector<int> v{};

mutex m;
cv oddCV;
cv evenCV;

void weaveOdd(void *a) {
  m.lock();
  for (int i = 1; i <= 9; i += 2) {
    while (v.size() % 2 == 1) {
      oddCV.wait(m);
    }
    cout << "odd says" << i << endl;
    v.push_back(i);
    evenCV.signal();
  }
  m.unlock();
}

void weaveEven(void *a) {
  m.lock();
  for (int i = 0; i <= 8; i += 2) {
    while (v.size() % 2 == 0) {
      evenCV.wait(m);
    }
    cout << "even says" << i << endl;
    v.push_back(i);
    oddCV.signal();
  }
  m.unlock();
}

void parent(void *a) {
  auto arg = reinterpret_cast<intptr_t>(a);

  m.lock();
  cout << "parent called with arg " << arg << endl;
  m.unlock();

  printf("Thread 1 created\n");
  thread t1(weaveOdd, nullptr);
  printf("Thread 2 created\n");
  thread t2(weaveEven, nullptr);
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}
