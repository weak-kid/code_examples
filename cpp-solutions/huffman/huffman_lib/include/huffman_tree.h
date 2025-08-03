#pragma once
#include "bit_code.h"
#include "bit_reader.h"
#include "bit_writer.h"
#include "consts.h"
#include "set"
#include "vector"

#include <array>

struct huffman_tree {
  struct Node {
    unsigned char symbol = 0;
    Node* left();
    Node* right();
    Node* lr[2] = {nullptr, nullptr};
  };

public:
  huffman_tree();
  explicit huffman_tree(std::array<u_int64_t, consts::CHARS_COUNT>);
  ~huffman_tree();

  // void make_tree();
  void make_codes();
  u_int64_t get_freq(unsigned char);
  const bit_code& get_code(unsigned char);
  Node* get_root();

private:
  void make_codes_impl(Node*);
  void destroy(Node*);

  Node* root;
  bit_code curr_code;
  std::array<u_int64_t, consts::CHARS_COUNT> heights;
  std::array<bit_code, 256> all_codes;
};
