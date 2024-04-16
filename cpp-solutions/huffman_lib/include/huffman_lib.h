#pragma once
#include "counter.h"
#include "huffman_tree.h"
#include "serialization.h"

namespace huffman {
void compress(std::istream& in, std::ostream& out);
void decompress(std::istream& in, std::ostream& out);
} // namespace huffman
