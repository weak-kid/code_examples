#include "huffman_lib.h"

void huffman::compress(std::istream& in, std::ostream& out) {
  huffman_tree tree(counter::count(in));
  in.seekg(0, std::ios::beg);
  serialize::compress(in, out, tree);
}

void huffman::decompress(std::istream& in, std::ostream& out) {
  serialize::decompress(in, out);
}
