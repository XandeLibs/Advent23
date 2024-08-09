#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

#define noop (int)0

const bool _DEBUG = 0;
int line = 1;

using string = std::string;

inline int get_number(string const& top, int pos, int* num_count){
  int num_found;
  if(_DEBUG) printf("Reading pos %d: ", pos);

  if(*num_count > 2) return 1;

  if(isdigit(top[pos])){
    do{
      pos--;

      if(pos < 0) break;
    }while (isdigit(top[pos])); 

  pos++;
  }

  num_found = atoi(&top[pos]);
  num_found = abs(num_found);
  if (num_found > 0) (*num_count)++;
  if (num_found == 0) num_found = 1;

  if(_DEBUG) printf("found %d, count %d\n", num_found, *num_count);

  return num_found;
}

inline int read_line(string const* top, string const* current, string const* bottom){
  if(_DEBUG) printf("------Line %d------\n", line++);
  int prod = 1;
  int prod_sum = 0;
  int pos = 0;
  //up = if the line above exists, left = if the column to the left exists etc
  bool up = (top != NULL)? true: false;
  bool down = (bottom != NULL)? true: false;
  bool left, right;

  for (auto& c : *current){
    if (c == '*'){
      prod = 1;
      int num_count = 0;
      left = pos>0? true: false;
      right = pos<139? true: false;

      //if the up char is a digit, atoi would get any digits on the left and the right
      if(up){
        if (_DEBUG) printf("Reading up at pos %d\n", pos);
        if(isdigit((*top)[pos])){
          prod *= get_number(*top, pos, &num_count);
        }
        else {
          if(left) prod *= get_number(*top, pos-1, &num_count);
          if(right)prod *= get_number(*top, pos+1, &num_count);
        }
      }

      if(down){
        if (_DEBUG) printf("Reading down at pos %d\n", pos);
        if(isdigit((*bottom)[pos])){
          prod *= get_number(*bottom, pos, &num_count);
        }
        else{
          if(left) prod *= get_number(*bottom, pos-1, &num_count);
          if(right)prod *= get_number(*bottom, pos+1, &num_count);
        }
      }

      if(left) prod *= get_number(*current, pos-1, &num_count);
      if(right)prod *= get_number(*current, pos+1, &num_count);

      if(num_count == 2){
        if(_DEBUG) printf("Exact 2 numbers, product %d\n", prod);
        prod_sum += prod;
      }
    }

    pos++;
  }

  if(_DEBUG) printf("prod_sum %d\n", prod_sum);

  return prod_sum;
}

int main (int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  int sum = 0;
  string top;
  string current;
  string bottom;

  getline(input, top);
  getline(input, current);
  sum += read_line(NULL, &top, &current);

  while (!input.eof()){
    getline(input, bottom);
    sum += read_line(&top, &current, &bottom);
    top = current;
    current = bottom;
    if(_DEBUG) printf("current sum %d\n", sum);
  }

  sum += read_line(&top, &current, NULL);

  output << sum;

  return 0;
}

