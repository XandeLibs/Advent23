#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

#define noop (int)0

const bool _DEBUG = 0;
int line = 1;

using string = std::string;

inline int get_number(string const& top, int pos){
  if(_DEBUG) printf("Reading pos %d: ", pos);

  if(isdigit(top[pos])){
    do{
      pos--;

      if(pos < 0) break;
    }while (isdigit(top[pos])); 

  pos++;
  }

  if(_DEBUG) printf("found %d\n", atoi(&top[pos]));

  return atoi(&top[pos]);
}

inline int read_line(string const* top, string const* current, string const* bottom){
  if(_DEBUG) printf("------Line %d------\n", line++);
  int sum = 0;
  int pos = 0;
  //up = if the line above exists, left = if the column to the left exists etc
  bool up = (top != NULL)? true: false;
  bool down = (bottom != NULL)? true: false;
  bool left, right;

  for (auto& c : *current){
    if (ispunct(c) && c != '.'){
      left = pos>0? true: false;
      right = pos<139? true: false;

      //if the up char is a digit, atoi would get any digits on the left and the right
      if(up){
        if(isdigit((*top)[pos])){
          sum += get_number(*top, pos);
        }
        else {
          if(left) sum += get_number(*top, pos-1);
          if(right)sum += get_number(*top, pos+1);
        }
      }

      if(down){
        if(isdigit((*bottom)[pos])){
          sum += get_number(*bottom, pos);
        }
        else{
          if(left) sum += get_number(*bottom, pos-1);
          if(right)sum += get_number(*bottom, pos+1);
        }
      }

      if(left) sum += get_number(*current, pos-1);
      if(right)sum += get_number(*current, pos+1);
    }
    pos++;
  }

  return sum;
}

int main (int argc, char *argv[]) {
  std::ofstream output(argv[2]);
  std::ifstream input(argv[1]);
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
  }

  sum += read_line(&top, &current, NULL);

  output << sum;

  return 0;
}

