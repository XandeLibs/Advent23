#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
  char line[64];
  FILE* input;
  int i_first = 0;
  int i_last = 0;
  int sum = 0;
  char combined_num[2];

  //read input
  input = fopen(argv[1], "r");

  while((i_last = getline(&line, 64, input)) != -1){
    i_first = -1;

    //get first digit
    while (!isdigit(line[++i_first]));

    //get last digit
    while(!isdigit(line[--i_last]));

    //combine
    combined_num[0] = line[i_last];
    combined_num[1] = line[i_first];

    //sum and loop back
    sum += atoi(combined_num);
  }

  printf("%i", sum);

  return EXIT_SUCCESS;
}
