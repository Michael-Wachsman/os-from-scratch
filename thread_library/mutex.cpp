#include "mutex.h"
#include "cpu.h"
#include "managed_uc.h"
#include "thread.h"
#include <cassert>
#include <cstdio>
#include <memory>
#include <stdexcept>

mutex::mutex() {}

mutex::~mutex() {}

void mutex::lock() {
  cpu::self()->interrupt_disable();
  cpu::self()->guard_lock();

  if (!free) {
    // add current thread to waiting queue
    swap_self_to_queue(waiting_queue);
  } else {
    // set status to busy
    free = false;
  }
  owner = (*cpu::self()->last_context)->id;

  cpu::self()->guard_unlock();
  cpu::self()->interrupt_enable();
}

void mutex::unlock() {
  cpu::self()->interrupt_disable();
  cpu::self()->guard_lock();

  release_lock();

  cpu::self()->guard_unlock();
  cpu::self()->interrupt_enable();
}

// used by both mutex::unlock and cv::wait
void mutex::release_lock() {
  if (free) {
    cpu::self()->guard_unlock();
    cpu::self()->interrupt_enable();
    throw std::runtime_error("Attempted to unlock already free lock");
  }

  if ((*cpu::self()->last_context)->id != owner) {
    cpu::self()->guard_unlock();
    cpu::self()->interrupt_enable();
    throw std::runtime_error("Attempted to free other thread's lock");
  }
  free = true;

  // special unused ID, so no one can unlock in between
  owner = 0;

  // check if waiting queue is empty
  if (!waiting_queue.empty()) {
    // move next thread in line to the ready queue
    std::unique_ptr<managed_uc> next_ready_context(
        waiting_queue.front().release());
    waiting_queue.pop_front();
    free = false;
    add_to_ready_queue(next_ready_context);
  }
}
