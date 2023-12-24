#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

const bool _DEBUG = 1;

using vector = std::vector<long long>;
using array3 = std::array<long long, 3>;
using vector3 = std::vector<array3>;

enum almanac_list {
  seed,
  soil,
  fert,
  water,
  light,
  temp,
  humid,
  loc
};

std::string almanac_names[loc+1] = {
  "seed",
  "soil",
  "fert",
  "water",
  "light",
  "temp",
  "humid",
  "loc"
};

vector mapConvert(std::ifstream &input, const vector &source);

vector convert(const vector &source, 
               const vector &src_start, 
               const vector &dst_start, 
               const vector &range);

void buildMap(std::ifstream &input, 
              vector &src_start, 
              vector &dest_start, 
              vector &range);

void print_vector(std::string message, vector vec);

int main (int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  std::string line, word;
  long long result = 0;
  vector vec1, vec2;
  vector *almanac[loc + 1];

  std::getline(input, line);
  ss.str(line);
  ss >> word; // Word seed

  almanac[0] = &vec1; // Seed
  almanac[1] = &vec2; // Soil

  // Builds seed list -------------------------
  while(ss >> word){
    (*almanac[0]).push_back(atoll(word.c_str()));
  }
  // ------------------------------------------

  if(_DEBUG) print_vector("Seeds: ", *almanac[0]);
  if(_DEBUG) std::cout << '\n' << "Starting map convesions\n";

  std::getline(input, line); // Empty line
  
  // Converts using all the maps except location
  for(int i = 1; i < 7; i++){
    if(_DEBUG) std::cout << "Converting from " << almanac_names[i-1] << " to " << almanac_names[i] << '\n';

    *almanac[i] = mapConvert(input, *almanac[i-1]);

    almanac[i-1]->clear();
    almanac[i+1] = almanac[i-1];
  }

  if(_DEBUG) std::cout << "Converting to location\n";

  *almanac[loc] = mapConvert(input, *almanac[humid]);

  result = *std::min_element(almanac[loc]->begin(), almanac[loc]->end());

  output << result << '\n';

  return 0;
}

void print_vector(std::string message, vector vec){
  std::cout << message;

  for(auto i: vec){
    std::cout << i << " ";
  }

  std::cout << '\n';
}

vector mapConvert(std::ifstream &input, const vector &source){
  vector source_range, dest_range, range, destination;

  buildMap(input, source_range, dest_range, range);

  destination = convert(source, source_range, dest_range, range);

  if(_DEBUG) print_vector("converted to: ", destination);
  if(_DEBUG) std::cout << "\n";

  return destination;

}

vector convert(const vector &source, const vector &source_range, const vector &dest_range, const vector &range){
  vector destination;
  long long pos, offset;

  for(auto s: source){
    // First value larger than s, if none, returns last
    auto ub = std::upper_bound(source_range.begin(), source_range.end(), s);

    if(ub == source_range.begin()){ // not in map
      if(_DEBUG) std::cout << s << " not in map\n";
      destination.push_back(s);
      continue;
    }

    ub = std::prev(ub);
    pos = std::distance(source_range.begin(), ub);

    if(s < *ub + range[pos]){ // Inside the source range
      if(_DEBUG) std::cout << s << " is between " << *ub << " and " << *ub + range[pos] << '\n';

      offset = s - *ub;
      destination.push_back(dest_range[pos] + offset);
    }
    else{

      if(_DEBUG) std::cout << s << " is not in the map, ub: " << *ub << " range: " << range[pos] << " max: " << *ub + range[pos] << "\n";
      destination.push_back(s);
    }

  }

  return destination;
}

void buildMap(std::ifstream &input, vector &source_range, vector &dest_range, vector &range){
  std::istringstream ss;
  std::string word, line;
  vector3 SDR;

  if(_DEBUG) std::cout << "Building map\n";

  std::getline(input, line); // Throw away "... map:" line
  std::getline(input, line);
  if(_DEBUG) std::cout << line << '\n';

  while(line.length() > 0){ //Empty line
    array3 map_line;
    ss.clear();
    ss.str(line);
    
    for(int i=0; ss >> word; i++){
      map_line[i] = atoll(word.c_str());
    }

    SDR.push_back(map_line); // Fills SDR with source, dest and range numbers
    std::getline(input, line);
  }
    
  auto sortSDR = [](array3 va, array3 vb) { return (va[1] < vb[1]); };

  std::sort(SDR.begin(), SDR.end(), sortSDR);
  if(_DEBUG) std::cout << "sorted map:\n";

  for(array3 num: SDR){
    if(_DEBUG) std::cout << num[0] << " " << num[1] << " " << num[2] << "\n";
    dest_range.push_back(num[0]);
    source_range.push_back(num[1]);
    range.push_back(num[2]);
  }

  if(_DEBUG) std::cout << "\n";
  if(_DEBUG) print_vector("map destinations: ", dest_range);
  if(_DEBUG) print_vector("map sources: ", source_range);
  if(_DEBUG) print_vector("map ranges: ", range);

}
