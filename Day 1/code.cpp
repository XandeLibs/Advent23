#include <cstdio>
#include <cctype>
#include <cstdlib>

int main(int argc, char *argv[])
{
  char* line;
  FILE* input;
  int i_first = 0;
  int i_last = 0;
  int sum = 0;
  char combined_num[2];
  size_t line_size = 64;

  line = new char[64]();
  //read input
  input = fopen(argv[1], "r");

  while((i_last = getline(&line, &line_size, input)) != -1){
    i_first = -1;

    //get first digit
    while (!isdigit(line[++i_first]));

    //get last digit
    while(!isdigit(line[--i_last]));

    //combine
    combined_num[0] = line[i_first];
    combined_num[1] = line[i_last];

    //sum and loop back
    sum += atoi(combined_num);
  }

  printf("%i\n", sum);

  delete line;

  return 0;
}
