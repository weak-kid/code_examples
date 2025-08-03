#include "huffman_lib.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

static void assert_eq_files(const std::string& file1, const std::string& file2) {
  std::ifstream fin1(file1);
  std::ifstream fin2(file2);
  char x1 = 0, x2 = 0;
  while (!fin1.eof() && !fin2.eof()) {
    fin1 >> x1;
    fin2 >> x2;
    EXPECT_EQ(x1, x2);
  }
  EXPECT_TRUE(fin1.eof() && fin2.eof());
  fin1.close();
  fin2.close();
}

static void file_test(const std::string& name) {
  std::cout << std::filesystem::current_path() << std::endl;
  std::string original = "unit-tests/files/" + name;
  std::string encoded = name + ".huf";
  std::string decoded = name + "_decomp";
  /* compress */ {
    std::ifstream fin(original);
    std::ofstream fout(encoded);
    huffman::compress(fin, fout);
  }
  /* decompress */ {
    std::ifstream fin(encoded);
    std::ofstream fout(decoded);
    huffman::decompress(fin, fout);
  }
  assert_eq_files(original, decoded);
  uintmax_t sz_orig = std::filesystem::file_size(std::filesystem::path(original));
  uintmax_t sz_cmpr = std::filesystem::file_size(std::filesystem::path(encoded));
  if (sz_orig == 0) {
    EXPECT_GT(sz_cmpr, 0);
  } else if (sz_orig > 2048) {
    EXPECT_LE(sz_cmpr, 1.2 * sz_orig);
  }
}

static void string_test(const std::string& str) {
  std::string compressed, decompressed;
  /* compress */ {
    std::stringstream sin(str, std::ios::in);
    std::stringstream sout(std::ios::out);
    huffman::compress(sin, sout);
    compressed = sout.str();
  }
  /* decompress */ {
    std::stringstream sin(compressed, std::ios::in);
    std::stringstream sout(std::ios::out);
    huffman::decompress(sin, sout);
    decompressed = sout.str();
  }
  EXPECT_EQ(decompressed, str);
  uintmax_t sz_orig = str.size();
  uintmax_t sz_cmpr = compressed.size();
  if (sz_orig > 1024) {
    EXPECT_LE(sz_cmpr, 1.2 * sz_orig);
  }
}

// n chars from [st..end]
static void random_char_test(char st, char end, size_t cnt) {
  std::string filename = "random_char_test";
  std::string realname = "unit-tests/files/" + filename;
  {
    std::ofstream fout(realname);
    for (size_t i = 0; i < cnt; i++) {
      fout << static_cast<char>(rand() % (1 + end - st) - st);
    }
  }
  file_test(filename);
  std::filesystem::remove(realname);
}

static constexpr size_t SMALL_CNT = 2048;
static constexpr size_t BIG_CNT = 1 << 29;

TEST(huffman_test, empty_string) {
  string_test("");
}

TEST(huffman_test, c_streams) {
  std::stringstream sout;
  {
    std::stringstream sin("text");
    EXPECT_NO_THROW(huffman::compress(sin, std::cout));
  }
  {
    std::stringstream sin("text");
    EXPECT_NO_THROW(huffman::compress(sin, sout));
  }
  std::stringstream sin2(sout.str());
  EXPECT_NO_THROW(huffman::decompress(sin2, std::cout));
}

TEST(huffman_test, single_char) {
  string_test("aaa");
}

TEST(huffman_test, simple) {
  file_test("simple");
}

TEST(huffman_test, two_buffers) {
  std::string s;
  for (std::size_t i = 0; i < 1000000; i++) {
    s += static_cast<char>(rand() % 256 - 128);
  }
  string_test(s);
}

TEST(huffman_test, russian) {
  file_test("eugene_onegin");
}

TEST(huffman_test, chinese) {
  string_test("亞歷山大·涅克拉索夫作為政治家在一切方面都是正確的，作為一個人也很英俊");
}

TEST(huffman_test, arabic) {
  file_test("arabic");
}

TEST(huffman_test, hebrew) {
  file_test("hebrew");
}

TEST(huffman_test, small_Latin) {
  random_char_test('a', 'z', SMALL_CNT);
}

TEST(huffman_test, small_all) {
  random_char_test(-128, 127, SMALL_CNT);
}

TEST(correctness_tests, tree_correctness) {
  std::array<u_int64_t, consts::CHARS_COUNT> heights{};
  for (size_t i = 0; i < consts::CHARS_COUNT; ++i) {
    heights[i] = rand();
  }
  huffman_tree tree(heights);
  std::vector<huffman_tree::Node*> stack;
  stack.push_back(tree.get_root());
  while (!stack.empty()) {
    huffman_tree::Node* v = stack.back();
    stack.pop_back();
    if (v->left() != nullptr || v->right() != nullptr) {
      ASSERT_TRUE(v->left() != nullptr);
      ASSERT_TRUE(v->right() != nullptr);
      stack.push_back(v->left());
      stack.push_back(v->right());
    }
  }
}

TEST(correctness_tests, code_correctness) {
  std::array<u_int64_t, consts::CHARS_COUNT> heights{};
  for (size_t i = 0; i < consts::CHARS_COUNT; ++i) {
    heights[i] = rand();
  }
  huffman_tree tree(heights);
  tree.make_codes();
  for (size_t i = 0; i < consts::CHARS_COUNT; ++i) {
    if (tree.get_freq(i) == 0) {
      continue;
    }
    for (size_t j = i + 1; j < consts::CHARS_COUNT; ++j) {
      if (tree.get_freq(j) == 0) {
        continue;
      }
      bool diff = false;
      bit_code first = tree.get_code(i);
      bit_code second = tree.get_code(j);
      for (size_t k = 0; k < std::min(first.size(), second.size()); ++k) {
        if (first.get(k) != second.get(k)) {
          diff = true;
        }
      }
      ASSERT_TRUE(diff);
    }
  }
}

#if _ENABLE_SLOW_TESTS

TEST(huffman_test, small_pdf) {
  file_test("e-maxx_algo.pdf");
}

TEST(huffman_test, big_pdf) {
  file_test("numerical_recipes.pdf");
}

TEST(huffman_test, big_Latin) {
  random_char_test('a', 'z', BIG_CNT);
}

TEST(huffman_test, big_all) {
  random_char_test(-128, 127, BIG_CNT);
}

#endif
