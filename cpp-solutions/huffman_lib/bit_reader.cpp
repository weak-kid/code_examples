#include "bit_reader.h"

bit_reader::bit_reader(std::istream& _in)
    : in(_in),
      buffer_size(0),
      buf_pos(0),
      byte_pos(consts::BYTESIZE),
      buffer(consts::BUFFSIZE, 0) {
  read_next_buff();
}

bool bit_reader::read_next() {
  read_next_buff();
  return buffer[buf_pos] & (1 << --byte_pos);
}

bool bit_reader::check_eof() {
  read_next_buff();
  return buffer_size == 0;
}

void bit_reader::read_next_buff() {
  if (byte_pos == 0) {
    byte_pos = consts::BYTESIZE;
    buf_pos++;
  }
  if (buffer_size == buf_pos) {
    buffer_size = in.rdbuf()->sgetn(buffer.data(), consts::BUFFSIZE);
    buf_pos = 0;
    byte_pos = consts::BYTESIZE;
  }
}

unsigned char bit_reader::read_char() {
  read_next_buff();
  unsigned char now = 0;
  if (byte_pos == consts::BYTESIZE) {
    now = buffer[buf_pos++];
    return now;
  }
  u_int8_t temp = byte_pos;
  now |= ((static_cast<unsigned char>(buffer[buf_pos]) & ((1 << byte_pos) - 1)) << (consts::BYTESIZE - byte_pos));
  byte_pos = 0;
  read_next_buff();
  now |= (static_cast<unsigned char>(buffer[buf_pos]) >> (temp));
  byte_pos = temp;
  return now;
}
