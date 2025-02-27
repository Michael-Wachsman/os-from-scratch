#include "../cpu.h"
#include "../mutex.h"
#include "../thread.h"

mutex m;

void parent(void *a) {
  m.lock();
}

int main() {
  cpu::boot(1, parent, reinterpret_cast<void *>(5), false, false, 0);
}
