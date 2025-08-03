#include "huffman_tree.h"

huffman_tree::Node* huffman_tree::Node::left() {
  return lr[0];
}

huffman_tree::Node* huffman_tree::Node::right() {
  return lr[1];
}

huffman_tree::huffman_tree() : root(nullptr) {}

huffman_tree::~huffman_tree() {
  if (root != nullptr) {
    destroy(root);
  }
}

void huffman_tree::destroy(huffman_tree::Node* vertex) {
  if (vertex->lr[0] != nullptr) {
    destroy(vertex->lr[0]);
  }
  if (vertex->lr[1] != nullptr) {
    destroy(vertex->lr[1]);
  }
  delete vertex;
}

huffman_tree::huffman_tree(std::array<u_int64_t, consts::CHARS_COUNT> heights) : heights(heights) {
  std::set<std::pair<u_int64_t, unsigned char>> heights_sorted;
  for (uint16_t i = 0; i < consts::CHARS_COUNT; ++i) {
    if (heights[i] != 0) {
      heights_sorted.insert({heights[i], i});
    }
  }
  Node* last = nullptr;
  if (heights_sorted.empty()) {
    root = new Node;
    return;
  }
  if (heights_sorted.size() == 1) {
    root = new Node;
    Node* fakenode = new Node;
    fakenode->symbol = heights_sorted.begin()->second;
    root->lr[0] = fakenode;
    return;
  }
  std::vector<Node*> nodes(consts::CHARS_COUNT);
  try {
    while (heights_sorted.size() > 1) {
      std::pair<int64_t, unsigned char> left = *heights_sorted.begin();
      heights_sorted.erase(heights_sorted.begin());
      std::pair<int64_t, unsigned char> right = *heights_sorted.begin();
      heights_sorted.erase(heights_sorted.begin());
      if (nodes[left.second] == nullptr) {
        nodes[left.second] = new Node;
        nodes[left.second]->symbol = left.second;
      }
      if (nodes[right.second] == nullptr) {
        nodes[right.second] = new Node;
        nodes[right.second]->symbol = right.second;
      }
      Node* res = new Node;
      res->lr[0] = nodes[left.second];
      res->lr[1] = nodes[right.second];
      res->symbol = std::min(left.second, right.second);
      nodes[left.second] = nullptr;
      nodes[right.second] = nullptr;
      nodes[std::min(left.second, right.second)] = res;
      heights_sorted.insert({left.first + right.first, std::min(left.second, right.second)});
      last = res;
    }
  } catch (...) {
    for (size_t i = 0; i < consts::CHARS_COUNT; ++i) {
      destroy(nodes[i]);
    }
    throw;
  }
  root = last;
}

void huffman_tree::make_codes() {
  curr_code.data.reserve(consts::CHARS_COUNT);
  std::fill(curr_code.data.begin(), curr_code.data.end(), 0);
  make_codes_impl(root);
}

void huffman_tree::make_codes_impl(Node* vertex) {
  if (vertex->lr[0] == nullptr && vertex->lr[1] == nullptr) {
    all_codes[vertex->symbol] = curr_code;
  }
  if (vertex->lr[0] != nullptr) {
    curr_code.push_back(0);
    make_codes_impl(vertex->lr[0]);
    curr_code.pop_back();
  }
  if (vertex->lr[1] != nullptr) {
    curr_code.push_back(1);
    make_codes_impl(vertex->lr[1]);
    curr_code.pop_back();
  }
}

u_int64_t huffman_tree::get_freq(unsigned char c) {
  return heights[c];
}

const bit_code& huffman_tree::get_code(unsigned char c) {
  return all_codes[c];
}

huffman_tree::Node* huffman_tree::get_root() {
  return root;
}
