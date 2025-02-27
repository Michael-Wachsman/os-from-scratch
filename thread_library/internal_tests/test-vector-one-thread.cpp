#include "../cpu.h"
#include "../thread.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

vector<int> v;

void helloWorld(void *a) {
  auto arg = reinterpret_cast<intptr_t>(a);
  for (int i = 0; i < arg; ++i) {
    v.push_back(i);
    printf("%d", i);
  }
  printf("\n");
  thread::yield();
  for (auto i : v) {
    printf("%d\n", i);
  }
}

int main(int argc, char *argv[]) {
  cpu::boot(1, helloWorld, reinterpret_cast<void *>(10), false, false, 0);
}
