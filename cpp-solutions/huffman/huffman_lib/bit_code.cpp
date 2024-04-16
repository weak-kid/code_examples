#include "bit_code.h"

void bit_code::push_back(uint8_t x) {
  temp |= (x << (consts::BYTESIZE - position - 1));
  position++;
  if (position == consts::BYTESIZE) {
    position = 0;
    data.push_back(temp);
    temp = 0;
  }
}

void bit_code::pop_back() {
  if (position == 0) {
    temp = data.back();
    data.pop_back();
    position = consts::BYTESIZE;
  }
  position--;
  temp &= ~(1 << (consts::BYTESIZE - position - 1));
}

size_t bit_code::size() const {
  return data.size() * consts::BYTESIZE + position;
}

bool bit_code::get(size_t ind) const {
  size_t bytes = ind / consts::BYTESIZE;
  unsigned char req_char;
  if (bytes == data.size()) {
    req_char = temp;
  } else {
    req_char = data[bytes];
  }
  ind %= consts::BYTESIZE;
  return req_char & (1 << (consts::BYTESIZE - ind - 1));
}
