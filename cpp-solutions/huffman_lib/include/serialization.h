#pragma once

#include "huffman_tree.h"
#include "iostream"

namespace serialize {
void compress(std::istream&, std::ostream&, huffman_tree& tree);
void decompress(std::istream&, std::ostream&);
void encode(bit_writer&, huffman_tree&);
void decode(bit_reader&, std::array<u_int64_t, consts::CHARS_COUNT>& heights);
} // namespace serialize
