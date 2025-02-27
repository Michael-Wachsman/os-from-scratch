#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <iterator>
#include <stdexcept>

mutex m;
cv c;

void start(void *_) {
  try {
    m.unlock();
  } catch (std::runtime_error &e) {
    std::cout << "error caught:" << e.what() << std::endl;
  }

  try {
    c.wait(m);
  } catch (std::runtime_error &e) {
    std::cout << "error caught:" << e.what() << std::endl;
  }

  printf("before first lock\n");
  m.lock();
  printf("before second lock\n");
  m.lock();
}

int main(int argc, char *argv[]) {
  cpu::boot(1, start, nullptr, false, false, 0);
  return 0;
}
