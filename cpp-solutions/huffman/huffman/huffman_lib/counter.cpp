#include "counter.h"

std::array<u_int64_t, consts::CHARS_COUNT> counter::count(std::istream& in) {
  if (!in.good()) {
    throw std::runtime_error("Error while opening file");
  }
  std::array<u_int64_t, consts::CHARS_COUNT> heights;
  std::fill(heights.begin(), heights.end(), 0);
  bit_reader reader(in);
  while (!reader.check_eof()) {
    ++heights[reader.read_char()];
  }
  return heights;
}
