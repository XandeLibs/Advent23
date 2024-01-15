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

using data = long long unsigned int;
using array2 = std::array<data, 2>;
using array3 = std::array<data, 3>;
using vector = std::vector<data>;
using vector2 = std::vector<array2>;
using vector3 = std::vector<array3>;
using almanac = std::array<std::array<vector, 3>, 8>;

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

const auto almanac_size = loc+1;

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

__global__ void minConvertedSeed_GPU(std::ifstream &input, const almanac &alm);

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
  vector2 seed_list;
  // 8 maps, each with 3 vectors(source_start, destination_start and range)
  almanac alm;

  std::getline(input, line);
  ss.str(line);
  ss >> word; // Word seed

  // Builds seed list -------------------------
  while(ss >> word){
    seed_range[0] = atoll(word.c_str());
    ss >> word;
    seed_range[1] = atoll(word.c_str());

    seed_list.push_back(seed_range);
  }
  // ------------------------------------------

  print_vector("Seeds: ", seed_list);

  std::getline(input, line); // Empty line

  // Reads and sorts all the maps
  for(auto i: {0, almanac_size-1}){
    buildMap(input, alm[i][0], alm[i][1], alm[i][2]);
  }

  // Copying data to GPU
  auto data_size = sizeof(data);
  auto seed_size = seed_list.size();
  size_t alm_size[almanac_size];

  for (auto i : {0, almanac_size-1})
    alm_size[i] = alm[i].size();

  data* g_seed_list;
  // Almanac, each array has different sizes, decided to do various 2D arrays instead of a big 3D one
  data* g_seed, *g_soil, *g_fert, *g_water, *g_light, *g_temp, *g_humid, *g_loc;
  data* g_seed_dst, *g_soil_dst, *g_fert_dst, *g_water_dst, *g_light_dst, *g_temp_dst, *g_humid_dst, *g_loc_dst;

  auto alloc_almanac = [alm, data_size](data* g_map, data* g_map_dst, int index){
    auto map_array = new data[alm[index][0].size()][2]; // Array with source and range
    auto height_bytes = alm[index][0].size()*data_size;

    memcpy(map_array[0], alm[index][0].data(), height_bytes); // Copy source
    memcpy(map_array[1], alm[index][2].data(), height_bytes); // Copy range

    cudaMemcpy2D(g_map, height_bytes, map_array, height_bytes, height_bytes, 2*data_size, cudaMemcpyDefault);

    // Copies dest_range
    cudaMemcpy(g_map_dst, alm[index][1].data(), height_bytes, cudaMemcpyDefault);
  };

  //cudaMemcpy(g_seed_list, seed_list.data(), seed_size*data_size*2, cudaMemcpyDefault);
  // tem que enviar só o qnt cada thread vai precisar, já que todas são iguais exceto uma
  alloc_almanac(g_seed, g_seed_dst, 0);
  alloc_almanac(g_soil, g_soil_dst, 1);
  alloc_almanac(g_fert, g_fert_dst, 2);
  alloc_almanac(g_water, g_water_dst, 3);
  alloc_almanac(g_light, g_light_dst, 4);
  alloc_almanac(g_temp, g_temp_dst, 5);
  alloc_almanac(g_humid, g_humid_dst, 6);
  alloc_almanac(g_loc, g_loc_dst, 7);
  // 17 total cudaMemcpy

  // -------------------

  output << result << '\n';

  auto end_clock = std::chrono::system_clock::now();
  auto time_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - start_clock).count();


  std::cout << "\nTime to execute: " << time_elapsed << " nanoseconds\n";

  return 0;
}

vector2 mapConvert(std::ifstream &input, const vector2 &source){
  vector source_range, dest_range, range;
  vector2 destination;

  buildMap(input, source_range, dest_range, range);

  destination = convert(source, source_range, dest_range, range);

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

    p_range_start = s;
    p_range_value = source_range;
    p_range_end = p_range_start + p_range_value - 1;

  NEXT_PARTIAL_RANGE: 
    p_range_end = p_range_start + p_range_value - 1;
    // First value larger than s, if none, returns last
    auto bound_str = std::upper_bound(map_src_start.begin(), map_src_start.end(), p_range_start);
    auto bound_end = std::upper_bound(map_src_start.begin(), map_src_start.end(), p_range_end);

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

      p_range_start = p_range_start + range_value;
      p_range_value = p_range_value - range_value;

      destination.push_back(new_split);
    };

    // Split is guaranteed 
    if(bound_str != bound_end){
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

  for(array3 num: SDR){
    source_range.push_back(num[1]);
    range.push_back(num[2]);
    dest_range.push_back(num[0]);
  }

}

__global__ void minConvertedSeed_GPU(std::ifstream &input, const almanac &alm){
  
}