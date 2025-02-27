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
cv c;

void child(void *_) {
  for (int i = 0; i < 5; ++i) {
    v.push_back(i);
  }
  cout << "Vector now has 5 elements" << endl;
  c.signal();
}

void parent(void *a) {
  thread t1(child, nullptr);
  m.lock();
  while (v.size() < 5) {
    cout << "Waiting for vector to have size 5" << endl;
    c.wait(m);
  }
  m.unlock();
  cout << "Finished waiting" << endl;
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}
