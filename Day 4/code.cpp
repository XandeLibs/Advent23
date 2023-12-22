#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

const bool _DEBUG = 1;

using vector = std::vector<int>;

int main (int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  std::string card, word;
  int sum = 0, card_score = -1;
  vector win, current;

  while(std::getline(input, card)){
    ss.str(card);
    ss >> word; // First word is Card
    ss >> word; // Second word is the card number, not used

    // Fills win vector with the winning numbers
    while( ss >> word && isdigit(*word.c_str())){
      win.push_back(atoi(word.c_str()));
    }

    // Fills current vector with numbers I have
    while(ss >> word){
      current.push_back(atoi(word.c_str()));
    }

    if(_DEBUG){
      std::cout << "winning numbers: ";
      
      for(auto i: win){
        std::cout << i << ' ';
      }
      std::cout << '\n';

      std::cout << "Numbers I have: ";

      for(auto i: current){
        std::cout << i << ' ';
      }
      std::cout << '\n';
    }

    // Win vector has size > log(current.lenght)
    // therefore, a sort and binary search is worth it
    // Of course, the input is small and this is mostly
    // for experimenting, no real impact
    std::sort(current.begin(), current.end());

    // Number of winning nums is always smaller
    for(auto i: win){
      if(std::binary_search(current.begin(), current.end(), i)){
        card_score++;
      }
    }

    if(_DEBUG) std::cout << card_score+1 << " numbers match\n";

    sum += pow(2, card_score);

    if(_DEBUG) std::cout << sum << " total points\n";

    // Cleanup
    card_score = -1;
    win.clear();
    current.clear();
    ss.clear();
  }

  output << sum << '\n';

  return 0;
}
