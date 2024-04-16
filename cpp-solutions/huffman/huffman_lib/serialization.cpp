#include "serialization.h"

void serialize::compress(std::istream& in, std::ostream& out, huffman_tree& tree) {
  if (!in.good()) {
    throw std::runtime_error("Error while opening file");
  }
  bit_reader reader(in);
  bit_writer writer(out);
  tree.make_codes();
  encode(writer, tree);
  while (!reader.check_eof()) {
    unsigned char c = reader.read_char();
    const bit_code& code = tree.get_code(c);
    for (size_t i = 0; i < code.data.size(); ++i) {
      writer.write_char(code.data[i]);
    }
    for (size_t j = 0; j < code.position; j++) {
      writer.write_bit(code.temp & (1 << (7 - j)));
    }
  }
}

void serialize::encode(bit_writer& writer, huffman_tree& tree) {
  u_int8_t allcode = 3;
  u_int16_t count8 = 0;
  std::vector<unsigned char> s8;
  u_int16_t count16 = 0;
  std::vector<unsigned char> s16;
  u_int16_t count32 = 0;
  std::vector<unsigned char> s32;
  u_int16_t count64 = 0;
  std::vector<unsigned char> s64;
  for (size_t i = 0; i < consts::CHARS_COUNT; ++i) {
    allcode += (tree.get_code(i).size() % consts::BYTESIZE) * (tree.get_freq(i) % consts::BYTESIZE) % consts::BYTESIZE;
    allcode %= consts::BYTESIZE;
    if (tree.get_freq(i) > 0 && tree.get_freq(i) < (1 << consts::BYTESIZE)) {
      count8++;
      s8.push_back(i);
    }
    if (tree.get_freq(i) >= (1 << consts::BYTESIZE) && tree.get_freq(i) < (1ll << (consts::BYTESIZE * 2))) {
      count16++;
      s16.push_back(i);
    }
    if (tree.get_freq(i) >= (1ll << (consts::BYTESIZE * 2)) && tree.get_freq(i) < (1ll << (consts::BYTESIZE * 4))) {
      count32++;
      s32.push_back(i);
    }
    if (tree.get_freq(i) >= (1ll << (consts::BYTESIZE * 4))) {
      count64++;
      s64.push_back(i);
    }
  }
  writer.write_bit(allcode & (1 << 2));
  writer.write_bit(allcode & (1 << 1));
  writer.write_bit(allcode & (1));
  for (; allcode % consts::BYTESIZE != 0; ++allcode) {
    writer.write_bit(false);
  }
  writer.write_char(count8 >> consts::BYTESIZE);
  writer.write_char(count8);
  writer.write_char(count16 >> consts::BYTESIZE);
  writer.write_char(count16);
  writer.write_char(count32 >> consts::BYTESIZE);
  writer.write_char(count32);
  writer.write_char(count64 >> consts::BYTESIZE);
  writer.write_char(count64);
  for (u_int8_t i = count64; count64-- > 0;) {
    writer.write_char(s64[count64]);
    for (int j = consts::BYTESIZE * (consts::BYTESIZE - 1); j >= 0; j -= consts::BYTESIZE) {
      writer.write_char(tree.get_freq(s64[count64]) >> (j));
    }
  }
  for (u_int8_t i = count32; count32-- > 0;) {
    writer.write_char(s32[count32]);
    for (int j = consts::BYTESIZE * 3; j >= 0; j -= consts::BYTESIZE) {
      writer.write_char(tree.get_freq(s32[count32]) >> (j));
    }
  }
  for (u_int8_t i = count16; count16-- > 0;) {
    writer.write_char(s16[count16]);
    for (int j = consts::BYTESIZE; j >= 0; j -= consts::BYTESIZE) {
      writer.write_char(tree.get_freq(s16[count16]) >> (j));
    }
  }
  for (u_int8_t i = count8; count8-- > 0;) {
    writer.write_char(s8[count8]);
    writer.write_char(tree.get_freq(s8[count8]));
  }
}

unsigned char get_char(huffman_tree::Node* v, bit_reader& reader) {
  if (v->lr[0] == nullptr && v->lr[1] == nullptr) {
    return v->symbol;
  }
  bool x = reader.read_next();
  return get_char(v->lr[x], reader);
}

void serialize::decompress(std::istream& in, std::ostream& out) {
  if (!in.good()) {
    throw std::runtime_error("Error while opening file");
  }
  std::array<u_int64_t, consts::CHARS_COUNT> heights;
  std::fill(heights.begin(), heights.end(), 0);
  bit_reader reader(in);
  bit_writer writer(out);
  decode(reader, heights);
  huffman_tree tree(heights);
  while (!reader.check_eof()) {
    writer.write_char(get_char(tree.get_root(), reader));
  }
}

void serialize::decode(bit_reader& reader, std::array<u_int64_t, consts::CHARS_COUNT>& heights) {
  u_int8_t off = 0;
  if (reader.read_next()) {
    off += 4;
  }
  if (reader.read_next()) {
    off += 2;
  }
  if (reader.read_next()) {
    off += 1;
  }
  for (; off % consts::BYTESIZE != 0; ++off) {
    if (reader.read_next()) {
      throw std::runtime_error("Not compressed file");
    }
  }
  u_int16_t count8 = (reader.read_char() << consts::BYTESIZE) + reader.read_char();
  u_int16_t count16 = (reader.read_char() << consts::BYTESIZE) + reader.read_char();
  u_int16_t count32 = (reader.read_char() << consts::BYTESIZE) + reader.read_char();
  u_int16_t count64 = (reader.read_char() << consts::BYTESIZE) + reader.read_char();
  for (size_t i = 0; i < count64; ++i) {
    unsigned char a = reader.read_char();
    for (int j = 0; j < consts::BYTESIZE * consts::BYTESIZE; j += consts::BYTESIZE) {
      heights[a] <<= consts::BYTESIZE;
      heights[a] += reader.read_char();
    }
  }
  for (size_t i = 0; i < count32; ++i) {
    unsigned char a = reader.read_char();
    for (int j = 0; j < consts::BYTESIZE * 4; j += consts::BYTESIZE) {
      heights[a] <<= consts::BYTESIZE;
      heights[a] += reader.read_char();
    }
  }
  for (size_t i = 0; i < count16; ++i) {
    unsigned char a = reader.read_char();
    for (int j = 0; j < consts::BYTESIZE * 2; j += consts::BYTESIZE) {
      heights[a] <<= consts::BYTESIZE;
      heights[a] += reader.read_char();
    }
  }
  for (size_t i = 0; i < count8; ++i) {
    unsigned char a = reader.read_char();
    heights[a] += reader.read_char();
  }
}
