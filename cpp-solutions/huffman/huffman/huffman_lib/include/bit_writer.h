#include "consts.h"

#include <ostream>

struct bit_writer {
  bit_writer(std::ostream&);
  bit_writer(const bit_writer&) = delete;
  bit_writer& operator=(const bit_writer&) = delete;
  ~bit_writer();

  void write_bit(bool);
  void write_char(unsigned char);

  void close();

private:
  std::ostream& out;
  std::string buffer;
  u_int16_t buffer_position;
  u_int8_t char_position;
  char current_char;

  void flush();
  void check_indices();
};

char to_char(u_int8_t);
u_int8_t from_char(char);
