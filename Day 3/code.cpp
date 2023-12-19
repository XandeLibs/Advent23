#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

using string = std::string;

#define noop (int)0

const bool _DEBUG = false;

namespace std {

inline int read_line(string const* top, string const* current, string const* bottom){
  int sum = 0;
  int pos = 0;
  bool up = (top != NULL)? true: false;
  bool down = (bottom != NULL)? true: false;
  bool left, right;

  for (auto& c : *current){
    if (ispunct(c)){
      left = pos>0? true: false;
      right = pos<139? true: false;

      
    }
    pos++;
  }

  return sum;
}

int main (int argc, char *argv[]) {
  ifstream input(argv[1]);
  int sum = 0;
  string line1;
  string line2;
  string line3;
  int line_size;
  size_t line_max_size = 141;

  getline(input, line1);
  getline(input, line2);
  sum += read_line(NULL, &line1, &line2);

  while (!input.eof()){
    getline(input, line3);
    sum += read_line(&line1, &line2, &line3);
    line1 = line2;
    line2 = line3;
  }

  sum += read_line(&line1, &line2, NULL);

  printf("%d\n", sum);

  return 0;
}

}
