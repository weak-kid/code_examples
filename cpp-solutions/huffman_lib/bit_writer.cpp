#include "bit_writer.h"

bit_writer::bit_writer(std::ostream& out)
    : out(out),
      char_position(consts::BYTESIZE),
      buffer_position(0),
      current_char(0),
      buffer(consts::BUFFSIZE, 0) {}

bit_writer::~bit_writer() {
  close();
}

void bit_writer::write_bit(bool bit) {
  --char_position;
  if (bit) {
    current_char |= (1 << char_position);
  }
  check_indices();
}

void bit_writer::write_char(unsigned char c) {
  size_t old_pos_c = char_position;
  current_char |= (c >> (consts::BYTESIZE - char_position));
  char_position = 0;
  check_indices();
  char_position = old_pos_c;
  current_char = ((c & ((1 << (consts::BYTESIZE - char_position)) - 1)) << char_position);
}

void bit_writer::flush() {
  out << buffer;
  char_position = consts::BYTESIZE;
  buffer_position = 0;
}

void bit_writer::check_indices() {
  if (char_position == 0) {
    buffer[buffer_position++] = current_char;
    char_position = consts::BYTESIZE;
    current_char = 0;
  }
  if (buffer_position == consts::BUFFSIZE) {
    flush();
  }
}

void bit_writer::close() {
  if (char_position == -1) {
    return;
  }
  if (char_position != consts::BYTESIZE) {
    buffer[buffer_position++] = current_char;
  }
  buffer.erase(buffer.begin() + buffer_position, buffer.end());
  flush();
  char_position = -1;
}
