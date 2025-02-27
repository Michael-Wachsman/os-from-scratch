#include "cv.h"
#include "cpu.h"
#include "managed_uc.h"
#include "mutex.h"
#include <memory>
#include <stdexcept>

cv::cv() {}

cv::~cv() {}

void cv::wait(mutex &m) {
  cpu::interrupt_disable();
  cpu::guard_lock();

  m.release_lock();

  swap_self_to_queue(waiting_queue);

  // after waking, reacquire mutex
  cpu::guard_unlock();
  cpu::interrupt_enable();

  // lock will handle the rest of toggling interrupts and guard
  m.lock();
}

void cv::signal() {
  cpu::interrupt_disable();
  cpu::guard_lock();

  if (!waiting_queue.empty()) {
    std::unique_ptr<managed_uc> next_ready_context(
        waiting_queue.front().release());
    waiting_queue.pop_front();
    add_to_ready_queue(next_ready_context);
  }

  cpu::guard_unlock();
  cpu::interrupt_enable();
}

void cv::broadcast() {
  cpu::interrupt_disable();
  cpu::guard_lock();

  add_to_ready_queue(waiting_queue);

  cpu::guard_unlock();
  cpu::interrupt_enable();
}
