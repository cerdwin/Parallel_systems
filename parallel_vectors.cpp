
 
#include <iostream>
#include <numeric>
#include <algorithm>
#include "SumsOfVectors.h"
#include <random>
 
void
sumsOfVectors_omp_per_vector(const vector<vector<int8_t>> &data, vector<long> &solution, unsigned long minVectorSize) {
    unsigned int vector_count = data.size();
    for(int i = 0; i < vector_count; i++){
        unsigned int vector_length = data[i].size();
        unsigned  int midsum = 0;
#pragma omp parallel for reduction(+:midsum)
 
        for(int x = 0; x < vector_length; x++){
            midsum += data[i][x];
        }
        solution[i] = midsum;
 
 
 
    }
}
 
void sumsOfVectors_omp_static(const vector<vector<int8_t>> &data, vector<long> &solution,
                              unsigned long minVectorSize) {
    unsigned int vector_count = data.size();
#pragma omp parallel for schedule(static)
    for(int i = 0; i < vector_count; i++){
        unsigned int vector_length = data[i].size();
        unsigned  int midsum = 0;
        for(int x = 0; x < vector_length; x++){
            midsum += data[i][x];
        }
        solution[i] = midsum;
 
 
 
    }
 
}
 
void sumsOfVectors_omp_dynamic(const vector<vector<int8_t>> &data, vector<long> &solution,
                               unsigned long minVectorSize) {
 
    unsigned int vector_count = data.size();
#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < vector_count; i++){
        unsigned int vector_length = data[i].size();
        unsigned  int midsum = 0;
        for(int x = 0; x < vector_length; x++){
            midsum += data[i][x];
        }
        solution[i] = midsum;
 
 
 
    }
}
 
void sumsOfVectors_omp_shuffle(const vector<vector<int8_t>> &data, vector<long> &solution,
                               unsigned long minVectorSize) {
    
    /* shuffling gathered from cpp reference at: https://en.cppreference.com/w/cpp/algorithm/iota */
    vector<int> index_list(data.size());
    std::iota(index_list.begin(), index_list.end(), 0.5);
    std::shuffle(index_list.begin(), index_list.end(), std::mt19937{std::random_device{}()});
    unsigned int vector_count = index_list.size();
    #pragma omp parallel for schedule(static)
    for(int i = 0; i < vector_count; i++){
        unsigned int vector_length = data[index_list[i]].size();
        unsigned  int midsum = 0;
        for(int x = 0; x < vector_length; x++){
            midsum += data[index_list[i]][x];
        }
        solution[index_list[i]] = midsum;
 
 
 
    }
}
