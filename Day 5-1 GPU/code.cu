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

const int Nthreads = 64;

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

const auto almanac_size = 7;
const auto MAP_MAX_SIZE = Nthreads;
const auto data_size = sizeof(data);    
const auto height_bytes = 47*data_size;

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

__device__ data g_alm[7][MAP_MAX_SIZE], g_alm_dst[7][MAP_MAX_SIZE][2];

__global__ void minConvertedSeed_GPU(array2* seed_list);

void buildMap(std::ifstream &input, 
              vector &src_start, 
              vector &dest_start, 
              vector &range);

void print_vector(std::string message, vector2 vec);

void alloc_almanac(almanac &alm, data (&g_map)[MAP_MAX_SIZE], data (&g_map_rngdst)[MAP_MAX_SIZE][2], int index);

int main (int argc, char *argv[]) {
  auto start_clock = std::chrono::system_clock::now();

  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);
  std::istringstream ss;
  std::string line, word;
  long long result = 0;
  array2 seed_range;
  vector2 seed_list;
  long long unsigned int seed_range_temp;
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

  std::getline(input, line); // Empty line
  
  cudaMalloc((void**)&g_alm, data_size * 7 * MAP_MAX_SIZE);
  cudaMalloc((void**)&g_alm_dst, data_size * 7 * MAP_MAX_SIZE * 2);

  // Reads and sorts all the maps, async copies to gpu
  for(auto i: {0, almanac_size-1}){
    buildMap(input, alm[i][0], alm[i][1], alm[i][2]);
    alloc_almanac(alm, g_alm[i], g_alm_dst[i], i);
    // 14 total cudaMemcpy
  }

  minConvertedSeed_GPU<<<Nthreads, seed_list.size()>>>(seed_list.data());
  // -------------------

  cudaFree(g_alm);
  cudaFree(g_alm_dst);

  output << result << '\n';

  auto end_clock = std::chrono::system_clock::now();
  auto time_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - start_clock).count();

  std::cout << "\nTime to execute: " << time_elapsed << " nanoseconds\n";

  return 0;
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

    // source now has end of range
    map_line[0] += map_line[2];

    SDR.push_back(map_line); // Fills SDR with source, dest and range numbers
    std::getline(input, line);
  }

  std::fill_n(SDR.end(), MAP_MAX_SIZE - SDR.size(), std::array<data, 3> {UINT64_MAX, UINT64_MAX, UINT64_MAX});

  auto sortSDR = [](array3 va, array3 vb) { return (va[1] < vb[1]); };

  std::sort(SDR.begin(), SDR.end(), sortSDR);

  for(array3 num: SDR){
    source_range.push_back(num[1]);
    range.push_back(num[2]);
    dest_range.push_back(num[0]);
  }

}

inline void alloc_almanac(almanac &alm, data (&g_map)[MAP_MAX_SIZE], data (&g_map_rngdst)[MAP_MAX_SIZE][2], int index){
    auto map_array = new data[alm[index][0].size()][2]; // Array with range and dest

    memcpy(map_array[0], alm[index][2].data(), height_bytes); // Copy range
    memcpy(map_array[1], alm[index][1].data(), height_bytes); // Copy dest

    cudaMemcpyToSymbolAsync(g_map, alm[index][0].data(), height_bytes);
    cudaMemcpyToSymbolAsync(g_map_rngdst, map_array, 2*height_bytes);
}

__global__ void minConvertedSeed_GPU(array2* seed_list){
  unsigned int tid = threadIdx.x;
  unsigned int bid = blockIdx.x;
  int offset = MAP_MAX_SIZE/2;
  unsigned int found_range[7];

  // copy maps to shared memory
  __shared__ data sh_g_alm[7][MAP_MAX_SIZE], sh_g_alm_dst[7][MAP_MAX_SIZE][2];
  
  for (auto i=0; i < 7; i++)
  {
    sh_g_alm[i][tid] = g_alm[i][tid];
  }

  for (auto i = 0; i < 7; i++)
  {
    sh_g_alm_dst[i][tid/2][tid%2] = g_alm_dst[i][tid/2][tid%2];

    tid += offset;
    sh_g_alm_dst[i][tid/2][tid%2] = g_alm_dst[i][tid/2][tid%2];
    tid -= offset;
  }
  
  __syncthreads();

  // calculate which interval and offset the thread will use
  auto seed_range = seed_list[bid];
  auto seed_amount = seed_range[1]/Nthreads;
  auto seed_start = seed_range[0] + (tid * seed_amount);
  auto seed_end = seed_start + seed_amount;

  if(tid == Nthreads-1){ // last thread
    seed_end = seed_range[1];
    seed_amount = seed_range[1] - seed_start;
  }

  // starts converting

  //first instance, creates found range

  /*começa com a lista vazia, popula uma vez,
  se nada mudar, continua usando o mesmo indice
  quando falhar na checagem inicial, os proximos serao zerados,
  talvez usar um goto para a parte de popular?
  se nao teriamos q ficar checando e manipulando o found_range*/

  auto upper_bound = [](const data (&first)[MAP_MAX_SIZE], const data value){
    auto f_index = 0;
    auto l_index = MAP_MAX_SIZE;
    data it;
    auto count = 64;
    int step;

    while(count > 0){
      it = first[f_index];
      step = count / 2;
      f_index += step;

      if(!(value < it)){
        
      }
    }

    return f_index;
  };

  for(auto seed = seed_start; seed <= seed_end; seed++){ // seed by seed, brute force method
    for(auto map_n = 0; map_n < 7; map_n++){             // for each map, seed to soil etc
      auto& g_src_end = g_alm[map_n][found_range[map_n]];
      auto& g_range = g_alm_dst[map_n][found_range[map_n]][0];
      auto& g_dest = g_alm_dst[map_n][found_range[map_n]][1];

      if(seed < g_src_end){
        seed = g_dest;
      }
      else
      {
        found_range[map_n]++;
        // Higher than start of the next interval, aka inside the next interval
        if(seed > g_src_end - g_range){
          seed = g_dest;
        }
        
        for(auto map_o = map_n; map_o < 7; map_o++){
          auto sdasd = (g_alm[map_n], g_alm[map_n], seed);
        }
        break;
      }
      
    }
  }    
}