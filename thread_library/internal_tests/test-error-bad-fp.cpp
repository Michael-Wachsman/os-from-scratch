#include "cpu.h"
#include "thread.h"

void make_null_thread(void *) { thread t(nullptr, nullptr); }

int main(int argc, char *argv[]) {
  cpu::boot(1, make_null_thread, nullptr, false, false, 0);
  return 0;
}
