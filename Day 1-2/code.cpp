#include <cstdio>
#include <cctype>
#include <cstdlib>

// one two three four five six seven eight nine zero
// one
// t -> wo/three
// f -> our/ive
// s -> ix/even
// eight
// nine
// zero
//
// e -> no/erht/vif/nin
// o -> wt/rez
// ruof
// xis
// neves
// thgie

inline char check_number(char* __restrict line, int line_size, bool backwards) {
  int i = backwards? line_size-2 : 0;

  while(true) {
    if(isdigit(line[i]))
      return line[i];
    else {
      if(backwards){
        switch (line[i]) {
          case 'e':
            switch (line[i-1]) {
              case 'n':
                if(line[i-2] == 'o')
                  return '1';

                if(line[i-2] == 'i' &&
                  line[i-3] == 'n')
                  return '9';

                break;
              case 'e':
                if(line[i-2] == 'r' &&
                  line[i-3] == 'h' &&
                  line[i-4] == 't')
                  return '3';

                break;
              case 'v':
                if(line[i-2] == 'i' &&
                  line[i-3] == 'f')
                  return '5';

                break;
            }

            break;
          case 'o':
            if(line[i-1] == 'w' &&
              line[i-2] == 't')
              return '2';

            if(line[i-1] == 'r' &&
              line[i-2] == 'e' &&
              line[i-3] == 'z')
              return '0';

            break;
          case 'r':
            if(line[i-1] == 'u' &&
              line[i-2] == 'o' &&
              line[i-3] == 'f')
              return '4';

            break;

          case 'x':
            if(line[i-1] == 'i' &&
              line[i-2] == 's')
              return '6';

            break;

          case 'n':
            if(line[i-1] == 'e' &&
              line[i-2] == 'v' &&
              line[i-3] == 'e' &&
              line[i-4] == 's')
              return '7';

            break;

          case 't':
            if(line[i-1] == 'h' &&
              line[i-2] == 'g' &&
              line[i-3] == 'i' &&
              line[i-4] == 'e')
              return '8';

            break;
        }
      }
      else {
        switch (line[i]) {
          case 'o':
            if(line[i+1] == 'n' &&
              line[i+2] == 'e')
              return '1';

            break;
          case 't':
            if(line[i+1] == 'w' &&
              line[i+2] == 'o')
              return '2';

            if(line[i+1] == 'h' &&
              line[i+2] == 'r' &&
              line[i+3] == 'e' &&
              line[i+4] == 'e')
              return '3';

            break;
          case 'f':
            if(line[i+1] == 'o' &&
              line[i+2] == 'u' &&
              line[i+3] == 'r')
              return '4';

            if(line[i+1] == 'i' &&
              line[i+2] == 'v' &&
              line[i+3] == 'e')
              return '5';

            break;

          case 's':
            if(line[i+1] == 'i' &&
              line[i+2] == 'x')
              return '6';

            if(line[i+1] == 'e' &&
              line[i+2] == 'v' &&
              line[i+3] == 'e' &&
              line[i+4] == 'n')
              return '7';

            break;

          case 'e':
            if(line[i+1] == 'i' &&
              line[i+2] == 'g' &&
              line[i+3] == 'h' &&
              line[i+4] == 't')
              return '8';

            break;

          case 'n':
            if(line[i+1] == 'i' &&
              line[i+2] == 'n' &&
              line[i+3] == 'e' )
              return '9';

            break;

          case 'z':
            if(line[i+1] == 'e' &&
              line[i+2] == 'r' &&
              line[i+3] == 'o')
              return '0';

            break;

          default:
            break;
        }
      }
    }
    backwards? i-- : i++;
  }

}

int main(int argc, char *argv[])
{
  char* line;
  FILE* input;
  int line_size = 0;
  int sum = 0;
  char combined_num[2];
  size_t line_max_size = 64;

  line = new char[64]();
  //read input
  input = fopen(argv[1], "r");

  while((line_size = getline(&line, &line_max_size, input)) != -1){
    //get first digit
    combined_num[0] = check_number(line, line_size, false);
    //printf("Encontrou %c\n", combined_num[0]);

    //get last digit
    combined_num[1] = check_number(line, line_size, true);
    //printf("Encontrou %c\n", combined_num[1]);

    //sum and loop back
    sum += atoi(combined_num);
    //printf("Combined: %d\n", atoi(combined_num));
  }

  printf("%i\n", sum);

  delete line;

  return 0;
}
