#include <cmath>
#include <cstdio>
#include <cstdlib>

#define noop (int)0

const bool _DEBUG = false;

int main (int argc, char *argv[]) {
  FILE* input;
  int id;
  int sum = 0;
  int cubes;
  char* line = new char[64];
  int line_size;
  size_t line_max_size = 64;

  input = fopen(argv[1], "r");

  while((line_size = getline(&line, &line_max_size, input)) != -1){
    //game id always in the same position
    id = atoi(&line[5]);
    _DEBUG? printf("id: %d\n", id):noop;

    //read each game, starting with the first number, always at line[8]
    for(int line_pos = 8+(floor(log10(id))); line[line_pos] != '\0'; line_pos += 2){
      cubes = atoi(&line[line_pos]);
      _DEBUG? printf("%d cubes ", cubes):noop;

      //jumps to the first letter of either red, blue or green
      //checks whether the number has 1 or 2 digits
      line[line_pos+1] == ' '? line_pos+=2: line_pos+=3;
      _DEBUG? printf("%c, ", line[line_pos]):noop;

      //checks the cube limits
      switch (line[line_pos]) {
        case 'r':
          if(cubes > 12)
            goto NEXTGAME;

          line_pos+= 3;
          break;
        case 'g':
          if(cubes > 13)
            goto NEXTGAME;

          line_pos += 5;
          break;
        case 'b':
          if(cubes > 14)
            goto NEXTGAME;

          line_pos += 4;
          break;
      }
    }

    _DEBUG? printf("\n"):noop;
    sum += id;

    NEXTGAME: continue;
  }

  printf("%d\n", sum);

  return 0;
}
