#include "cpu.h"
#include "managed_uc.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <memory>
#include <ucontext.h>

std::vector<cpu *> cpu::cpu_vec = std::vector<cpu *>();
std::deque<std::unique_ptr<managed_uc>> cpu::ready_queue =
    std::deque<std::unique_ptr<managed_uc>>();

void swap_self_to_queue(std::deque<std::unique_ptr<managed_uc>> &queue) {
  assert_interrupts_disabled();
  assert(cpu::guard.load());

  // push current context to waiting queue (TO BE UPDATED WITH SWAP)
  queue.push_back(std::move(*(cpu::self()->last_context)));
  assert(cpu::self()->last_context.get()->get() == nullptr);

  if (!cpu::ready_queue.empty()) {
    // use the front of the ready queue
    (cpu::self()->last_context)->swap(cpu::ready_queue.front());
    cpu::ready_queue.pop_front();

    swapcontext(&queue.back()->uc, &(*(cpu::self()->last_context))->uc);
  } else {
    // go to sleep
    swapcontext(&queue.back()->uc, &cpu::self()->my_goldfish->uc);
  }
}

void watch_suspend() {
  while (true) {
    // because we may resume inside this loop
    assert_interrupts_disabled();
    assert(cpu::self()->guard.load());

    // kill the last context hanging out before we sleep
    *(cpu::self()->to_kill) = nullptr;

    // when we jump here, context is stored in goldfish instead
    assert(cpu::self()->last_context->get() == nullptr);

    if (!cpu::ready_queue.empty()) {
      // take front of ready queue, and put it on cpu
      cpu::self()->last_context->swap(cpu::ready_queue.front());
      assert(cpu::ready_queue.front().get() == nullptr);
      assert(cpu::self()->last_context->get() != nullptr);
      cpu::ready_queue.pop_front();

      // we do need to swap context so that goldfish's stack is OK
      swapcontext(&cpu::self()->my_goldfish->uc,
                  &(*cpu::self()->last_context)->uc);
    } else {
      // no ready threads
      cpu::self()->is_idle = true;
      cpu::guard_unlock();
      cpu::self()->interrupt_enable_suspend();

      cpu::self()->interrupt_disable();
      cpu::guard_lock();
    }
  }
}

void add_to_ready_queue(unique_ptr<managed_uc> &context) {
  cpu::ready_queue.push_back(std::move(context));
  cpu::check_ready_and_wake();
}

void add_to_ready_queue(std::deque<std::unique_ptr<managed_uc>> &contexts) {
  for (auto &context : contexts) {
    cpu::ready_queue.push_back(std::move(context));
  }
  contexts.clear();
  cpu::check_ready_and_wake();
}

/*
 * The cpu constructor initializes a CPU.  It is provided by the thread
 * library and called by the infrastructure.  After a CPU is initialized, it
 * should run user threads as they become available.  If func is not
 * nullptr, cpu::cpu() also creates a user thread that executes func(arg).
 *
 * On success, cpu::cpu() should not return to the caller.
 */
cpu::cpu(thread_startfunc_t func, void *arg) {
  guard_lock(); // touching shared data
  interrupt_vector_table[TIMER] = timer_interrupt_handler;
  interrupt_vector_table[IPI] = ipi_handler;

  my_goldfish = std::make_shared<managed_uc>();
  makecontext(&my_goldfish->uc, reinterpret_cast<void (*)()>(watch_suspend), 0);

  to_kill = std::make_shared<unique_ptr<managed_uc>>(nullptr);
  last_context = std::make_shared<unique_ptr<managed_uc>>(nullptr);

  cpu_num = cpu_vec.size();
  cpu_vec.push_back(this);
  if (func) {
    thread t(func, arg, nullptr); // alternate constructor for multicpu

    is_idle = false; // not nessecary, adding for explicitness

    // thread constructor adds to ready queue
    std::unique_ptr<managed_uc> to_run(cpu::ready_queue.front().release());
    assert(to_run.get() != nullptr);
    cpu::ready_queue.pop_front();
    cpu::self()->last_context->swap(to_run);

    setcontext(&(*(cpu::self()->last_context))->uc);
  } else {
    // nothing to do yet, go to sleep
    is_idle = true;
    setcontext(&my_goldfish->uc);
  }
}

void cpu::guard_lock() {
  bool guard_state = guard.exchange(true);
  while (guard_state) {
    guard_state = guard.exchange(true);
  }
}

void cpu::guard_unlock() { guard.store(false); }

void timer_interrupt_handler() {
  // basically yield minus suspension
  cpu::self()->interrupt_disable();
  cpu::guard_lock();

  // to handle situation when interrupting after coming back from enable suspend
  if (!cpu::self()->is_idle and cpu::self()->last_context->get() != nullptr) {
    assert(cpu::self()->last_context->get() != nullptr);
    if (!cpu::ready_queue.empty()) {
      swap_self_to_queue(cpu::ready_queue);
    }
  }
  cpu::guard_unlock();
  cpu::self()->interrupt_enable();
}

void ipi_handler() {
  // no guarantees of any kind here
}

// checks ready queue and wakes up idle processors if they could be used
void cpu::check_ready_and_wake() {
  assert_interrupts_disabled();
  assert(cpu::guard.load());

  // there might be a nicer way to write
  int unhandled_threads = cpu::ready_queue.size();

  auto a_cpu_it = cpu::cpu_vec.begin();

  // find cpus that aren't idle and wake up
  // until there are no more idle cpus, or enough to handle current
  while (a_cpu_it != cpu::cpu_vec.end() and unhandled_threads > 0) {
    if ((**a_cpu_it).is_idle) {
      (**a_cpu_it).is_idle = false;
      (**a_cpu_it).interrupt_send();
      unhandled_threads--;
    }
    ++a_cpu_it;
  }
}
