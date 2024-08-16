#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using string = std::string;
using vecint = std::vector<int>;

class char_path {
  char origin[3];
  char left[3];
  char right[3];
};

class int_path {
  unsigned int left, right;
};

int main(int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  string line, word;
  string steps;
  string origin;
  int result = 0;
  std::vector<int_path> ipath;
  std::vector<char_path> cpath;

  std::getline(input, line);
  ss.str(line);
  ss >> steps;

  std::getline(input, line); // empty line
  ss.clear();

  while (std::getline(input, line)) {
    ss.str(line);

    ss >> origin;
    ss >> word; // "="
    ss >> word;
    auto left = word.substr(1, 3);
    ss >> word;
    auto right = word.substr(0, 3);

    ss.clear();
  }

  output << result << '\n';

  return 0;
}
