#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define DEBUG

#ifdef DEBUG
#define dbgWrite( str ) do { std::cout << str << std::endl; } while(false)
#else
#define dbgWrite( str ) do { } while(false)
#endif // DEBUG

using vector = std::vector<unsigned long long>;

int main (int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  std::string line, word, num_append;
  vector time, distance;
  unsigned long long result = 1;

  // Receiving input -------------------------
  std::getline(input, line);
  ss.str(line);
  ss >> word; // Word "Time: "

  while(ss >> word){
    num_append.append(word.c_str());
  }

  time.push_back(atoll(num_append.c_str()));

  ss.clear();
  num_append.clear();

  std::getline(input, line);
  ss.str(line);
  ss >> word; // Word "Distance: "

  while(ss >> word){
    num_append.append(word.c_str());
  }

  distance.push_back(atoll(num_append.c_str()));
  //----------------------------------------

  // p = time pressed, t = race time
  // Distance = p*(t-p)
  // -p^2 + pt - d = 0
  // p^2 - pt + d = 0
  // t is known, distance to beat is known, p can be found
  // sum = -b/a, sum = t
  // prod = c/a, prod = d
  // 

  auto distance_iter = distance.begin();
  unsigned long long middle, diff;
  for(auto t: time){
    auto d = *(distance_iter++);

    dbgWrite("Testing time " << t << " for record " << d);

    if(t%2 == 0){
      middle = (t/2)*(t/2);

      diff = ceil(sqrt(middle - d))-1;

      dbgWrite("T even, middle " << middle << " diff " << diff << " adding to result " << diff*2 + 1);
      
      result *= diff*2 + 1;
    }
    else {
      middle = (t/2)*(t/2 + 1);

      diff = ceil(sqrt(middle - d))-1;

      dbgWrite("T even, middle " << middle << " diff " << diff << " adding to result " << (diff+1)*2);

      result *= (diff+1)*2;
    }
  }

  output << result << '\n';

  return 0;
}
