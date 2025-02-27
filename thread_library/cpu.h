/*
 * cpu.h -- interface to the simulated CPU
 *
 * This interface is used mainly by the thread library.
 * The only part that is used by application programs is cpu::boot().
 *
 * This class is implemented almost entirely by the infrastructure.  The
 * responsibilities of the thread library are to implement the cpu constructor
 * (and any functions you choose to add) and manage interrupt_vector_table
 * (and any variables you choose to add).
 *
 * You may add new variables and functions to this class.  If you add variables
 * to the cpu class, add them at the *end* of the class definition, and do not
 * exceed the 1 KB total size limit for the class.
 *
 * Do not modify any of the given variable and function declarations.
 */

#pragma once

#include <array>
#include <sys/ucontext.h>
#if !defined(__cplusplus) || __cplusplus < 201700L
#error Please configure your compiler to use C++17 or C++20
#endif

#include "managed_uc.h"
#include "thread.h"
#include <atomic>
#include <cstddef>
#include <deque>
#include <memory>
#include <ucontext.h>
#include <vector>

using interrupt_handler_t = void (*)();
using std::shared_ptr;
using std::unique_ptr;

class cpu {
public:
  /*
   * cpu::boot() starts all CPUs in the system.  The number of CPUs is
   * specified by num_cpus.
   * One CPU will call cpu::cpu(func, arg); the other CPUs will call
   * cpu::cpu(nullptr, nullptr).
   *
   * On success, cpu::boot() does not return.
   *
   * async, sync, random_seed configure each CPU's generation of timer
   * interrupts (which are only delivered if interrupts are enabled).
   * Timer interrupts in turn cause the current thread to be preempted.
   *
   * The sync and async parameters can generate several patterns of
   * interrupts:
   *
   *     1. async = true: generate interrupts asynchronously every 1 ms
   * using SIGALRM.  These are non-deterministic.
   *
   *     2. sync = true: generate synchronous, pseudo-random interrupts on
   *        patterns by changing random_seed.
   *
   * An application will be deterministic if num_cpus=1 && async=false.
   */
  static void boot(unsigned int num_cpus, thread_startfunc_t func, void *arg,
                   bool async, bool sync, unsigned int random_seed);

  /*
   * interrupt_disable() disables interrupts on the executing CPU.  Any
   * interrupt that arrives while interrupts are disabled will be saved
   * and delivered when interrupts are re-enabled.
   *
   * interrupt_enable() and interrupt_enable_suspend() enable interrupts
   * on the executing CPU.
   *
   * interrupt_enable_suspend() atomically enables interrupts and suspends
   * the executing CPU until it receives an interrupt from another CPU.
   * The CPU will ignore timer interrupts while suspended.
   *
   * These functions should only be called by the thread library (not by
   * an application).  They are static member functions because they always
   * operate on the executing CPU.
   *
   * Each CPU starts with interrupts disabled.
   */
  static void interrupt_disable();
  static void interrupt_enable();
  static void interrupt_enable_suspend();

  /*
   * interrupt_send() sends an inter-processor interrupt to the specified CPU.
   * E.g., c_ptr->interrupt_send() sends an IPI to the CPU pointed to by c_ptr.
   */
  void interrupt_send();

  /*
   * The interrupt vector table specifies the functions that will be called
   * for each type of interrupt.  There are two interrupt types: TIMER and
   * IPI.
   */
  static constexpr unsigned int TIMER = 0;
  static constexpr unsigned int IPI = 1;
  interrupt_handler_t interrupt_vector_table[IPI + 1];

  static cpu *self(); // returns pointer to the cpu that
                      // the calling thread is running on

  /*
   * The infrastructure provides an atomic guard variable, which thread
   * libraries should use to provide mutual exclusion on multiprocessors.
   * The switch invariant for multiprocessors specifies that this guard variable
   * must be true when calling swapcontext.  guard is initialized to false.
   */
  static std::atomic<bool> guard;

  /*
   * Disable the default copy constructor, copy assignment operator,
   * move constructor, and move assignment operator.
   */
  cpu(const cpu &) = delete;
  cpu &operator=(const cpu &) = delete;
  cpu(cpu &&) = delete;
  cpu &operator=(cpu &&) = delete;

  /*
   * The cpu constructor initializes a CPU.  It is provided by the thread
   * library and called by the infrastructure.  After a CPU is initialized, it
   * should run user threads as they become available.  If func is not
   * nullptr, cpu::cpu() also creates a user thread that executes func(arg).
   *
   * On success, cpu::cpu() should not return to the caller.
   */
  cpu(thread_startfunc_t func, void *arg);

  /************************************************************
   * Add any variables you want here (do not add them above   *
   * interrupt_vector_table).  Do not exceed the 1 KB size    *
   * limit for this class.  Do not add any private variables. *
   ************************************************************/

  static void guard_lock();
  static void guard_unlock();
  // could be outside the class if desired, safety of static keyword is helpful
  static void check_ready_and_wake();
  static std::vector<cpu *> cpu_vec;
  static std::deque<std::unique_ptr<managed_uc>> ready_queue;
  std::shared_ptr<managed_uc> my_goldfish;

  // cannot add straight up unique ptrs to cpp class, since it must be copyable
  // thus, creating shared_ptrs to a unique_ptr, to maintain context owned by
  // one thing at a time invariant
  shared_ptr<unique_ptr<managed_uc>> last_context;
  // last stack that will be killed next time we switch
  shared_ptr<unique_ptr<managed_uc>> to_kill;
  bool is_idle = false;
  int cpu_num;
};
static_assert(sizeof(cpu) <= 1024);
static_assert(
    std::is_standard_layout<cpu>::value); // ISSUE BEFORE SUBMIT with unique
static_assert(offsetof(cpu, interrupt_vector_table) == 0);

/*
 * assert_interrupts_disabled() and assert_interrupts_enabled() can be used
 * as error checks inside the thread library.  They will assert (i.e. abort
 * the program and dump core) if the condition they test for is not met.
 */
#define assert_interrupts_disabled()                                           \
  assert_interrupts_private(__FILE__, __LINE__, true)
#define assert_interrupts_enabled()                                            \
  assert_interrupts_private(__FILE__, __LINE__, false)

/*
 * assert_interrupts_private() is a private function for the interrupt
 * functions.  Your thread library should not call it directly.
 */
void assert_interrupts_private(const char *, int, bool);

void timer_interrupt_handler();

void ipi_handler();

void watch_suspend();

void swap_self_to_queue(std::deque<std::unique_ptr<managed_uc>> &queue);

void add_to_ready_queue(unique_ptr<managed_uc> &context);
void add_to_ready_queue(std::deque<std::unique_ptr<managed_uc>> &contexts);