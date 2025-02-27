#pragma once
#include <array>
#include <cstdio>
#include <memory>
#include <ucontext.h>
static constexpr unsigned int STACK_SIZE =
    262144; // size of each thread's stack in byte

struct managed_uc {
  ucontext_t uc;
  static unsigned int max_id;
  unsigned int id;
  std::unique_ptr<std::array<char, STACK_SIZE>> stack;
  managed_uc();

  ~managed_uc();
  // { printf("Destroyed managed context\n"); }
};
