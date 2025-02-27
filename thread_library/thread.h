/*
 * thread.h -- interface to the thread library
 *
 * This file should be included by the thread library and by application
 * programs that use the thread library.
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once

#include "managed_uc.h"
#include <deque>
#include <memory>
#include <sys/ucontext.h>

#if !defined(__cplusplus) || __cplusplus < 201700L
#error Please configure your compiler to use C++17 or C++20
#endif

// now reorganized in managed_uc
//  static constexpr unsigned int STACK_SIZE =
//      262144; // size of each thread's stack in byte

using thread_startfunc_t = void (*)(void *);

struct shared_thread_info {
  std::deque<std::unique_ptr<managed_uc>> join_waiting_queue;
  bool alive;
};

class thread {
public:
  // create a new thread
  thread(thread_startfunc_t func, void *arg);
  // create a new thread in an already locked context
  thread(thread_startfunc_t func, void *arg, void *_);
  void construct_thread(thread_startfunc_t func, void *arg);

  ~thread();

  void join(); // wait for this thread to finish

  static void yield(); // yield the CPU

  /*
   * Disable the copy constructor and copy assignment operator.
   */
  thread(const thread &) = delete;
  thread &operator=(const thread &) = delete;

  /*
   * Move constructor and move assignment operator.  Implementing these is
   * optional in Project 2.
   */
  thread(thread &&);
  thread &operator=(thread &&);
  std::shared_ptr<shared_thread_info> info;
};
