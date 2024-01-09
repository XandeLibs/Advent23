#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

const int HAND_SIZE = 5;
const char JOKER = '.';

using string = std::string;
using pair = std::pair<string, uint>;
using pair_vector = std::vector<pair>;

enum h_type {
  high_card,
  one_pair,
  two_pair,
  three_kind,
  full_house,
  four_kind,
  five_kind
};

h_type findType(string hand){
  char prev = '0';
  int dup_count = 0, dup_count2 = 0;
  int joker_count = 0;
  std::sort(hand.begin(), hand.end());

  std::cout << "Analysing " << hand.c_str() << "\n";

  // Hands can have at most 2 set of duplicates, forming either a
  // two pair or a full house
  int* dup = &dup_count;
  for(auto h: hand){
    if(h == JOKER)
      joker_count++;

    if(h == prev){
      (*dup)++;
    }
    else if (dup_count) {
      dup = &dup_count2;
    }

    prev = h;
  }

  std::cout << "Dups: " << dup_count << " " << dup_count2 << "\n";

  dup_count++;
  dup_count2++;

  switch (dup_count) {
    case 5:
      return five_kind;

    case 4:
      if(joker_count)
        return five_kind;

      return four_kind;

    case 3:
      if(dup_count2 == 2){
        if(joker_count)
          return five_kind;

        return full_house;
      }

      if(joker_count)
        return four_kind;
  
      return three_kind;

    case 2:
      if(dup_count2 == 3){
        if(joker_count)
          return five_kind;

        return full_house;
      }

      if(dup_count2 == 2){
        if(joker_count == 2) // one of the pairs is jokers
          return four_kind;
        else if(joker_count) // joker is the lone card
          return full_house;

        return two_pair;
      }

      if(joker_count)
        return three_kind;

      return one_pair;

    default:
      if(joker_count)
        return one_pair;

      return high_card;
  }

}

int main (int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  std::string line, word;
  pair_vector five, four, full, three, two, one, high;
  pair_vector* all_hands[five_kind+1] = {&high, &one, &two, &three, &full, &four, &five};
  pair hand;
  h_type hand_type;
  pair hand_pair;
  unsigned long long result = 0;
  unsigned long long rank = 1;

  while (std::getline(input, line)) {

    ss.str(line);

    ss >> word; // Hand
    
    std::cout << word.c_str() << "\n";

    //Replaces with chars in lexical order for easier sorting
    std::replace(word.begin(), word.end(), 'T', 'B');
    std::replace(word.begin(), word.end(), 'J', JOKER);
    std::replace(word.begin(), word.end(), 'Q', 'D');
    std::replace(word.begin(), word.end(), 'K', 'E');
    std::replace(word.begin(), word.end(), 'A', 'F');

    hand_type = findType(word);
    hand_pair.first = word;

    ss >> word; // Bid

    hand_pair.second = atoi(word.c_str());
    all_hands[hand_type]->push_back(hand_pair);

    ss.clear();
  }

  // Sort by strength
  auto hands_sort = [](const pair& p1, const pair& p2){
      return (p1.first < p2.first);
  };

  for(auto& hand_vec: all_hands){
    std::sort(hand_vec->begin(), hand_vec->end(), hands_sort);

    std::cout << "Sorted hand: \n";

    for(auto& hand: *hand_vec){
      std::cout << hand.first.c_str() << " ";
    }

    std::cout << "\n";

  }

  //Add it all up
  for(auto& hand_vec: all_hands){
    for(auto& hand: *hand_vec){
      std::cout << "Adding " << hand.second*rank << "\n";
      result += hand.second * rank;
      rank++;
    }
  }

  output << result << '\n';

  return 0;

}
