/*
 * mutex.h -- interface to the mutex class
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#include "managed_uc.h"
#include <deque>
#include <memory>
#include <ucontext.h>

#pragma once

class mutex {
public:
  mutex();
  ~mutex();

  void lock();
  void unlock();
  void release_lock();

  /*
   * Disable the copy constructor and copy assignment operator.
   */
  mutex(const mutex &) = delete;
  mutex &operator=(const mutex &) = delete;

  /*
   * Move constructor and move assignment operator.  Implementing these is
   * optional in Project 2.
   */
  mutex(mutex &&);
  mutex &operator=(mutex &&);

  // Should not be static since it's per mutex
  std::deque<std::unique_ptr<managed_uc>> waiting_queue;
  // If the thread who owns this lock dies, no one else can grab the lock
  unsigned int owner = 0;
  bool free = true;
};
