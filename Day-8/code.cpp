#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

using string = std::string;
using vecint = std::vector<int>;

bool zeroes = false;

vecint last_nums;

vecint getDiffs(vecint &vec) {
  vecint next_line;
  auto num_prev = vec[0];
  zeroes = true;

  for (auto &num : vec | std::views::drop(1)) {
    next_line.push_back(num - num_prev);

    if ((num - num_prev) != 0)
      zeroes = false;

    num_prev = num;
  }

  std::cout << num_prev << " last num\n";

  last_nums.push_back(num_prev);

  return next_line;
}

int main(int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  string line, word;
  vecint sequence;
  int result = 0;

  while (std::getline(input, line)) {
    ss.str(line);
    zeroes = false;

    while (!ss.eof()) {
      ss >> word;
      sequence.push_back(atoi(word.c_str()));
    }

    auto &vec = sequence;
    while (!zeroes) {
      vec = getDiffs(vec);
    }

    result = std::accumulate(last_nums.begin(), last_nums.end(), result);

    last_nums.clear();
    sequence.clear();

    std::cout << result << '\n';

    ss.clear();
  }

  output << result << '\n';

  return 0;
}
