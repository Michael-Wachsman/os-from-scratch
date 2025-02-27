#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <vector>

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
    v.push_back(i);
    printf("odd says %d\n", i);
    // evenCV.signal();
  }
  m.unlock();
}

void weaveEven(void *a) {
  m.lock();
  for (int i = 0; i <= 8; i += 2) {
    while (v.size() % 2 == 0) {
      evenCV.wait(m);
    }
    v.push_back(i);
    printf("even says %d\n", i);
    // oddCV.signal();
  }
  m.unlock();
}

void parent(void *a) {
  auto arg = reinterpret_cast<intptr_t>(a);

  m.lock();
  printf("parent called with arg %ld\n", arg);
  m.unlock();

  thread t1(weaveOdd, nullptr);
  thread t2(weaveEven, nullptr);
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}
