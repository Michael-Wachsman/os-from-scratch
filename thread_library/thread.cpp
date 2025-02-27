#include <memory>

#include "cpu.h"
#include "managed_uc.h"
#include "thread.h"
#include <cassert>
#include <cstdio>
#include <deque>
#include <sys/ucontext.h>
#include <ucontext.h>

using std::make_shared;
using std::shared_ptr;
using std::unique_ptr;

void wrapped_function(thread_startfunc_t func, void *arg, void *shared_data) {
  // shenanigans to pass a shared ptr through a void *
  unique_ptr<shared_ptr<shared_thread_info>> u_ptr(
      reinterpret_cast<shared_ptr<shared_thread_info> *>(shared_data));

  // pulling down to stack to simplify syntax
  shared_ptr<shared_thread_info> local_info = *u_ptr;
  u_ptr = nullptr;

  cpu::guard_unlock();
  cpu::self()->interrupt_enable();
  func(arg);
  cpu::self()->interrupt_disable();
  cpu::guard_lock();

  assert(local_info.get() != nullptr);

  // set active state of execution to false
  local_info->alive = false;

  // add elements in join waiting queue to ready queue
  add_to_ready_queue(local_info->join_waiting_queue);

  // remove ptrs from join queue, make sure nothing is held onto
  local_info = nullptr;

  // we can't kill this stack till we leave, so leave it to be killed later
  *(cpu::self()->to_kill) = nullptr;
  cpu::self()->to_kill.swap(cpu::self()->last_context);

  // set to beginning of ready queue, or go to sleep
  if (!cpu::ready_queue.empty()) {
    cpu::self()->last_context->swap(cpu::ready_queue.front());
    assert(cpu::ready_queue.front().get() == nullptr);
    assert(cpu::self()->last_context.get() != nullptr);
    cpu::ready_queue.pop_front();

    setcontext(&(*cpu::self()->last_context)->uc);
  } else {
    setcontext(&cpu::self()->my_goldfish->uc);
  }
}

void thread::construct_thread(thread_startfunc_t func, void *arg) {
  try {
    auto context = std::make_unique<managed_uc>();
    // initialize shared state between thread obj and stream of execution
    info = make_shared<shared_thread_info>();
    info->alive = true;

    // unique ptr shenanigans to pass shared_ptr through primitives
    auto u_p = std::make_unique<shared_ptr<shared_thread_info>>(info);

    makecontext(&context->uc, reinterpret_cast<void (*)()>(wrapped_function), 3,
                func, arg, u_p.release());
    add_to_ready_queue(context);
    assert(context.get() == nullptr);
  } catch (std::bad_alloc &e) {
    cpu::guard_unlock();
    cpu::self()->interrupt_enable();
    throw e;
  }
}

thread::thread(thread_startfunc_t func, void *arg) {
  cpu::self()->interrupt_disable();
  cpu::guard_lock();
  construct_thread(func, arg);
  cpu::guard_unlock();
  cpu::self()->interrupt_enable();
}

// alternate constructor, provided by overloading, can be called without
// dropping lock and interrupts
// used inside cpu boot
thread::thread(thread_startfunc_t func, void *arg, void *_) {
  assert(cpu::guard.load());
  assert_interrupts_disabled();
  //some unnessecary checks, but cleans up code
  construct_thread(func, arg);
}

thread::~thread() {}

void thread::join() {
  cpu::self()->interrupt_disable();
  cpu::guard_lock();

  assert(cpu::self()->last_context.get() != nullptr);

  // if execution is not finished, add self to waiting queue
  if (info->alive) {
    swap_self_to_queue(info->join_waiting_queue);
  }

  cpu::guard_unlock();
  cpu::self()->interrupt_enable();
}

void thread::yield() {
  cpu::self()->interrupt_disable();
  cpu::guard_lock();

  if (!cpu::ready_queue.empty()) {
    swap_self_to_queue(cpu::ready_queue);
  }

  cpu::guard_unlock();
  cpu::self()->interrupt_enable();
}
