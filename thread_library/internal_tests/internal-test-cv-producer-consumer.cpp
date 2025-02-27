#include "../cpu.h"
#include "../cv.h"
#include "../mutex.h"
#include "../thread.h"
#include <cassert>
#include <deque>
#include <iostream>
#include <memory>

using std::deque;
using std::make_shared;

#define RED "\e[31m"
#define GREEN "\e[32m"
#define YELLOW "\e[33m"
#define MAGENTA "\e[35m"
#define CYAN "\e[36m"
#define RESET "\e[0m"

deque<int> coke_machine{};
size_t MAX_SIZE = 10;
int coke_num = 0;
int producer_threads_active = 0;

std::shared_ptr<thread> c1_ptr;
std::shared_ptr<thread> c2_ptr;
std::shared_ptr<thread> p1_ptr;
std::shared_ptr<thread> p2_ptr;
std::shared_ptr<thread> p3_ptr;

mutex lock;
cv produceCV;
cv consumeCV;

void consumer(void *arg) {
  auto id = reinterpret_cast<intptr_t>(arg);
  while (producer_threads_active > 0) {
    printf("%sConsumer%ld attempts to grab lock%s\n", YELLOW, id, RESET);
    lock.lock();
    printf("%sConsumer%ld grabs lock%s\n", CYAN, id, RESET);
    while (coke_machine.size() != MAX_SIZE) {
      if (producer_threads_active < 1) {
        break;
      }
      printf("Consumer%ld relinquishes lock\n", id);
      consumeCV.wait(lock);
      printf("%sConsumer%ld grabs lock after waiting%s\n", CYAN, id, RESET);
    }
    printf("%sConsumer%ld consumed coke%d%s\n", RED, id, coke_machine.front(),
           RESET);
    coke_machine.pop_front();
    if (producer_threads_active > 0) {
      produceCV.broadcast();
    } else {
      printf("%sConsumer%ld noticed producers stopped%s\n", MAGENTA, id, RESET);
      lock.unlock();
      break;
    }
    lock.unlock();
  }
  printf("%sConsumer%ld tries to grab lock in phase 2%s\n", YELLOW, id, RESET);
  lock.lock();
  printf("%sConsumer%ld grabbed lock\n%s", CYAN, id, RESET);
  while (!coke_machine.empty()) {
    printf("%sConsumer%ld consumed coke%d in phase 2%s\n", RED, id,
           coke_machine.front(), RESET);
    coke_machine.pop_front();
    if (coke_machine.front() == 9) {
      lock.unlock();
      c2_ptr->join();
    }
  }
  printf("Consumer%ld done\n", id);
}

void producer(void *arg) {
  auto id = reinterpret_cast<intptr_t>(arg);
  for (int i = 1; i < 6; ++i) {
    printf("%sProducer%ld attempts to grab lock%s\n", YELLOW, id, RESET);
    lock.lock();
    printf("%sProducer%ld grabs lock%s\n", CYAN, id, RESET);
    if (i % 2 == 0) {
      thread::yield();
    }
    while (coke_machine.size() == MAX_SIZE) {
      printf("Producer%ld waiting for coke to be consumed\n", id);
      produceCV.wait(lock);
    }
    coke_machine.push_back(coke_num);
    printf("%sProducer%ld produced coke%d%s\n", GREEN, id, coke_num, RESET);
    coke_num++;
    consumeCV.signal();
    lock.unlock();
  }
  producer_threads_active--;
  assert(producer_threads_active >= 0);
  printf("Producer%ld done\n", id);
}

void scheduler(void *a) {
  producer_threads_active = 3;
  c1_ptr = make_shared<thread>(consumer, reinterpret_cast<void *>(1));
  p1_ptr = make_shared<thread>(producer, reinterpret_cast<void *>(1));
  p2_ptr = make_shared<thread>(producer, reinterpret_cast<void *>(2));
  c2_ptr = make_shared<thread>(consumer, reinterpret_cast<void *>(2));
  p3_ptr = make_shared<thread>(producer, reinterpret_cast<void *>(3));
}

int main() {
  cpu::boot(4, scheduler, reinterpret_cast<void *>(5), true, true, 0);
}
