#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <iostream>
#include <stdexcept>

cv c;
mutex lock;

void baby(void *_) {
  printf("Baby attempts to grab lock.\n");
  lock.lock();
  printf("Baby grabs lock\n");
  lock.unlock();
  printf("Baby signals\n");
  c.signal();
}

void child(void *_) {
  printf("Child attempts to grab lock.\n");
  lock.lock();
  thread(baby, nullptr);
  printf("Child acquired lock and now waits.\n");
  c.wait(lock);
  printf("Child releases lock\n");
  lock.unlock();
  printf("Child signals\n");
  c.signal();
}

void sibling(void *_) {
  try {
    printf("Sibling attempts to steal parent's lock!\n");
    c.wait(lock);
  } catch (std::runtime_error &e) {
    printf("Sibling could not steal parent's lock\n");
  }
  printf("Sibling attempts to grab lock normally.\n");
  lock.lock();
  printf("Sibling acquired lock and waits\n");
  c.wait(lock);
  printf("Sibling releases lock\n");
}

void parent(void *arg) {
  thread(child, nullptr);
  thread(sibling, nullptr);
  lock.lock();
  printf("Parent acquired lock!\n");
  thread::yield();
  printf("Parent releases lock\n");
  lock.unlock();
}

int main(int argc, char *argv[]) {
  cpu::boot(1, parent, nullptr, false, false, 0);
  return 0;
}
