#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
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
  int match_count = 0, copies_sum;
  uint card_id = 0;
  vector win, current, copies;

  int card_count = std::count(std::istreambuf_iterator<char>(input),
             std::istreambuf_iterator<char>(), '\n');

  copies.resize(card_count+1);

  input.seekg(0);

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
        match_count++;
      }
    }

    if(_DEBUG) std::cout << match_count << " numbers match\n";

    copies[card_id]++;

    if(_DEBUG) std::cout << card_id+1 << " has " << copies[card_id] << " copies\n";

    if(_DEBUG) std::cout << "added " << copies[card_id] << " copies to ";

    while(match_count){
      if(_DEBUG) std::cout << match_count + card_id << ", ";

      copies[match_count + card_id] += copies[card_id];
      match_count--;
    }
    if(_DEBUG) std::cout << '\n';

    card_id++;

    // Cleanup
    match_count = 0;
    win.clear();
    current.clear();
    ss.clear();
  }

  copies.resize(card_id);

  copies_sum = std::accumulate(copies.begin(), copies.end(), 0);

  output << copies_sum << '\n';

  return 0;
}
