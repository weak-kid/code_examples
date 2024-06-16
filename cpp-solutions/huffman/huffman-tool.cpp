#include "huffman_lib.h"
#include "huffman_tree.h"

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
  if (argc != 1 && argc != 2 && argc != 6) {
    throw std::runtime_error("WRONG ARGUMENTS");
  }
  std::vector<std::string> args;
  for (size_t i = 0; i < argc; ++i) {
    args.push_back(std::string(argv[i]));
  }
  if (argc == 2 || argc == 1) {
    if (args[1] == "--help" || argc == 1) {
      std::cout << "Allowed options:\n"
                   "  --help                Description of options\n"
                   "  --compress            Compressing mode, from input to output, can't work in \n"
                   "                        compress and decompress mode in the same time\n"
                   "  --decompress          Decompressing mode, from input to output, can't work in\n"
                   "                        compress and decompress mode in the same time\n"
                   "  --input arg           path to input-file\n"
                   "  --output arg          path to output-file\n";
      return 0;
    }
  }
  bool is_comp = false;
  bool is_decomp = false;
  bool is_input = false;
  bool is_output = false;
  size_t iter = 1;
  std::string input_name, output_name;
  while (iter < 6) {
    if (args[iter] == "--compress") {
      is_comp = true;
    }
    if (args[iter] == "--decompress") {
      is_decomp = true;
    }
    if (args[iter] == "--input") {
      is_input = true;
      iter++;
      input_name = args[iter];
    }
    if (args[iter] == "--output") {
      is_output = true;
      iter++;
      output_name = args[iter];
    }
    iter++;
  }
  if (is_comp && is_decomp) {
    throw std::runtime_error("Wrong args format, write --help for help");
  }
  if (output_name == input_name) {
    throw std::runtime_error("Can't read and write in same file");
  }
  std::ifstream input;
  input.open(input_name);
  std::ofstream output;
  output.open(output_name);
  if (!input.is_open()) {
    throw std::runtime_error("Can't open input file");
  }
  if (!output.is_open()) {
    throw std::runtime_error("Can't create output file");
  }
  if (is_comp) {
    huffman::compress(input, output);
  }
  if (is_decomp) {
    huffman::decompress(input, output);
  }
}
