#pragma once

#include "consts.h"
#include "cstddef"
#include "cstdint"
#include "vector"

struct bit_code {
  void push_back(uint8_t x);
  void pop_back();
  size_t size() const;
  bool get(size_t) const;

  std::vector<char> data;
  char temp = 0;
  uint8_t position = 0;
};
