#include <array>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#define DEBUG

#ifdef DEBUG
#define dbgWrite( str ) do { std::cout << str << std::endl; } while(false)
#else
#define dbgWrite( str ) do { } while(false)
#endif // DEBUG

using array2 = std::array<long long, 2>;
using array3 = std::array<long long, 3>;
using vector = std::vector<long long>;
using vector2 = std::vector<array2>;
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

vector2 mapConvert(std::ifstream &input, const vector2 &source);

vector2 convert(const vector2 &source, 
                const vector &src_start, 
                const vector &dst_start, 
                const vector &range);

void buildMap(std::ifstream &input, 
              vector &src_start, 
              vector &dest_start, 
              vector &range);

void print_vector(std::string message, vector2 vec);

int main (int argc, char *argv[]) {
  auto start_clock = std::chrono::system_clock::now();

  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  std::string line, word;
  long long result = 0;
  array2 seed_range;
  vector2 vec1, vec2;
  vector2 *almanac[loc + 1];

  std::getline(input, line);
  ss.str(line);
  ss >> word; // Word seed

  almanac[0] = &vec1; // Seed
  almanac[1] = &vec2; // Soil

  // Builds seed list -------------------------
  while(ss >> word){
    seed_range[0] = atoll(word.c_str());
    ss >> word;
    seed_range[1] = atoll(word.c_str());

    (*almanac[0]).push_back(seed_range);
  }
  // ------------------------------------------

  print_vector("Seeds: ", *almanac[0]);
  dbgWrite('\n' << "Starting map convesions\n");

  std::getline(input, line); // Empty line

  // Converts using all the maps except location
  for(int i = 1; i < 7; i++){
    dbgWrite("Converting from " << almanac_names[i-1] << " to " << almanac_names[i] << '\n');

    *almanac[i] = mapConvert(input, *almanac[i-1]);

    almanac[i-1]->clear();
    almanac[i+1] = almanac[i-1];
  }

  dbgWrite("Converting to location\n");

  *almanac[loc] = mapConvert(input, *almanac[humid]);

  auto locComp = [](const array2& a1, const array2& a2) { return (a1[0] < a2[0]); };

  auto min = std::min_element(almanac[loc]->begin(), almanac[loc]->end(), locComp);

  result = (*min)[0];

  output << result << '\n';

  auto end_clock = std::chrono::system_clock::now();
  auto time_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - start_clock).count();


  std::cout << "\nTime to execute: " << time_elapsed << " nanoseconds\n";

  return 0;
}

void print_vector(std::string message, vector2 vec){
  #ifdef DEBUG
  dbgWrite(message);

  for(auto i: vec){
    dbgWrite("[" << i[0] << ", " << i[1] << "] ");
  }

  dbgWrite("");
  #endif // DEBUG
}

vector2 mapConvert(std::ifstream &input, const vector2 &source){
  vector source_range, dest_range, range;
  vector2 destination;

  buildMap(input, source_range, dest_range, range);

  destination = convert(source, source_range, dest_range, range);

  print_vector("converted to: ", destination);
  dbgWrite("");

  return destination;

}

vector2 convert(const vector2 &source, const vector &map_src_start, const vector &map_dst_start, const vector &map_range){
  vector2 destination;
  long long pos, offset, p_range_end;
  array2 partial_range;
  auto& p_range_start = partial_range[0];
  auto& p_range_value = partial_range[1];

  // Conversion occurs in ranges
  // If the start and the end have differente upper bounds, a split is guaranteed
  // It just needs to find the split point
  // If they have the same upper bounds, a split may not happen
  // A check for if both are inside the map range needs to be done
  for(auto& [s, source_range]: source){
    dbgWrite("\nNew range from source");

    p_range_start = s;
    p_range_value = source_range;
    p_range_end = p_range_start + p_range_value - 1;

  NEXT_PARTIAL_RANGE: 
    p_range_end = p_range_start + p_range_value - 1;
    dbgWrite("Range: " << p_range_start << ", " << p_range_end);
    // First value larger than s, if none, returns last
    auto bound_str = std::upper_bound(map_src_start.begin(), map_src_start.end(), p_range_start);
    auto bound_end = std::upper_bound(map_src_start.begin(), map_src_start.end(), p_range_end);

    dbgWrite("str bound start: " << *bound_str << ", end bound start: " << *bound_end);

    auto split = [&](auto range_value, bool inside_map){
      array2 new_split;

      if(inside_map){
        new_split[0] = map_dst_start[pos] + offset;
        new_split[1] = map_range[pos] - offset;
      }
      else{
        new_split[0] = p_range_start;
        new_split[1] = range_value;
      }

      dbgWrite("Split " << inside_map << " range value: " << range_value << "\nFirst range: " << new_split[0] << ", " << new_split[1]);

      p_range_start = p_range_start + range_value;
      p_range_value = p_range_value - range_value;

      dbgWrite("Second range: " << p_range_start << ", " << p_range_value);

      destination.push_back(new_split);
    };

    // Split is guaranteed 
    if(bound_str != bound_end){
      dbgWrite("Different bounds");
      // Out of map before first range
      if(bound_str == map_src_start.begin()){
        split(*map_src_start.begin() - p_range_start, false);
        goto NEXT_PARTIAL_RANGE;
      }

      bound_str--;
      pos = std::distance(map_src_start.begin(), bound_str);
      offset = p_range_start - *bound_str;

      // Inside map in bound_str range
      if(p_range_start < *bound_str + map_range[pos]-1){
        split(*bound_str + map_range[pos] - p_range_start, true);
        goto NEXT_PARTIAL_RANGE;
      }

      // Outside map after bound_str range
      bound_str++;
      split(*bound_str - p_range_start, false);
      goto NEXT_PARTIAL_RANGE;
    }

    // Same upper bounds
    // Both out of map
    if(bound_str == map_src_start.begin()){
      destination.push_back(partial_range);
      continue;
    }

    bound_str--;
    bound_end--;

    pos = std::distance(map_src_start.begin(), bound_str);
    offset = p_range_start - *bound_str;

    dbgWrite("range start is " << p_range_start << " end of bound is " << *bound_str + map_range[pos]-1);
    // Start is inside map
    if(p_range_start < *bound_str + map_range[pos]-1){
      // Both inside
      if(p_range_end < *bound_str + map_range[pos]-1){
        p_range_start = map_dst_start[pos] + offset;
        destination.push_back(partial_range);
      }
      else{
        split(*bound_str + map_range[pos] - p_range_start, true);
        goto NEXT_PARTIAL_RANGE;
      }
    }
    // Both outside map
    else{
      destination.push_back(partial_range);
    }
  }

  return destination;
}

void buildMap(std::ifstream &input, vector &source_range, vector &dest_range, vector &range){
  std::istringstream ss;
  std::string word, line;
  vector3 SDR;

  std::getline(input, line); // Throw away "... map:" line
  std::getline(input, line);

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
  dbgWrite("sorted map: S R D");

  for(array3 num: SDR){
    dbgWrite(num[1] << " " << num[2] << " " << num[0] );
    source_range.push_back(num[1]);
    range.push_back(num[2]);
    dest_range.push_back(num[0]);
  }

  dbgWrite("");
}
