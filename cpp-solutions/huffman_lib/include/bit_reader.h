#pragma once
#include "consts.h"

#include <istream>

struct bit_reader {
  explicit bit_reader(std::istream&);
  bit_reader(const bit_reader&) = delete;
  bit_reader& operator=(const bit_reader&) = delete;
  ~bit_reader() = default;

  bool read_next();
  bool check_eof();
  unsigned char read_char();

private:
  void read_next_buff();
  std::string buffer;
  u_int16_t buffer_size;
  std::istream& in;
  u_int16_t buf_pos;
  u_int8_t byte_pos;
};
