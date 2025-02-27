// testing a thread getting passed through and simple functionality

#include "../cpu.h"
#include "../thread.h"
#include <iostream>
#include <memory>
#include <vector>
using std::cout;
using std::vector;

void test(void *vec_ptr) {
  // making sure things get all the way through
  vector<int> *data = reinterpret_cast<vector<int> *>(vec_ptr);
  for (auto e : *data) {
    printf("%d ", e);
  }
  printf("\n");
}

void boot(void *data) {
  auto datum = reinterpret_cast<long int>(data);
  printf("got value passed in: %ld\n", datum);
  std::unique_ptr<std::vector<int>> complex =
      std::make_unique<std::vector<int>>(std::vector<int>({4, 6, 8, 7}));
  thread t(test, reinterpret_cast<void *>(complex.get()));
  t.join();
}

int main(int argc, char *argv[]) {
  cpu::boot(1, boot, reinterpret_cast<void *>(388), false, false, 0);
  return 0;
}
