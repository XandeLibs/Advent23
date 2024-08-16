#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using string = std::string;
using bytevec = std::vector<uint8_t>;

int main(int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  string line, word;
  bytevec sequence;
  int result = 0;

  while (std::getline(input, line)) {
    ss.str(line);

    while (!ss.eof()) {
      ss >> word;
      sequence.push_back(atoi(word.c_str()));
    }

    ss.clear();
  }

  output << result << '\n';

  return 0;
}
