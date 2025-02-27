#include "../cpu.h"
#include "../thread.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

vector<int> v(5, 0);

void addN(void *index) {
  auto n = reinterpret_cast<intptr_t>(index);

  for (int i = 0; i < 10; ++i) {
    v[n]++;
  }
}

void parent(void *a) {
  thread t0(addN, reinterpret_cast<void *>(0));
  thread t1(addN, reinterpret_cast<void *>(1));
  thread t2(addN, reinterpret_cast<void *>(2));
  thread t3(addN, reinterpret_cast<void *>(3));
  thread t4(addN, reinterpret_cast<void *>(4));
  t0.join();
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  printf("%d\n", v[0]);
  printf("%d\n", v[1]);
  printf("%d\n", v[2]);
  printf("%d\n", v[3]);
  printf("%d\n", v[4]);
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}
