#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <memory>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

int i = 1;

void hello(void *_) { i = 0; }

void boot(void *in_n) {
  try {
    for (int i = 0; i < 1000; ++i) {
      thread(hello, nullptr);
    }
  } catch (std::bad_alloc &e) {
    std::cout << "caught a bad alloc error:" << e.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {

  cpu::boot(1, boot, reinterpret_cast<void *>(445), false, false, 0);
  return 0;
}
