#include "managed_uc.h"

#include <array>
#include <cstdio>
#include <memory>
#include <ucontext.h>

//0 IS RESERVED ID
unsigned int managed_uc::max_id = 1;

managed_uc::managed_uc() {
  stack = std::make_unique<std::array<char, STACK_SIZE>>();
  uc.uc_stack.ss_sp = stack.get()->data();
  uc.uc_stack.ss_size = STACK_SIZE;
  uc.uc_stack.ss_flags = 0;
  uc.uc_link = nullptr;
  id = max_id;
  max_id++;
}

managed_uc::~managed_uc() {}
