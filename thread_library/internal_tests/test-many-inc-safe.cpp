// not yet changed
#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

mutex m;

int g = 0;
bool open = true;

void addN(void *in_n) {
  auto n = reinterpret_cast<intptr_t>(in_n);
  m.lock();
  cout << n << endl;
  m.unlock();

  for (int i = 0; i < n; ++i) {
    m.lock();
    g++;
    cout << g << endl;
    m.unlock();
  }
}

void parent(void *a) {
  int total_threads = 10;
  std::vector<thread> threads;
  // this resize is issue, no implicit constructor
  // threads.reserve(total_threads);

  for (int i = 0; i < total_threads; ++i) {
    threads.emplace_back(thread(addN, reinterpret_cast<void *>(i)));
  }

  for (auto &t : threads) {
    t.join();
    m.lock();
    cout << g << endl;
    m.unlock();
  }
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}
