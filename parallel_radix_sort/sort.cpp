#include "sort.h"
#include <iostream>
#include <typeinfo>
using namespace std;
 
 
void second_radix( std::vector<std::string *> &vector_to_sort, const MappingFunction &mapping_Function,
         unsigned long alphabet_size, unsigned long string_lengths, const unsigned int index) {
    if (index != string_lengths){
        std::vector<std::string *> buckets[alphabet_size];
        int i = 0;
        for(auto item = vector_to_sort.begin(); item != vector_to_sort.end(); ++item)
        {
            const unsigned long step = mapping_Function((*item)->at(index));
            buckets[step].push_back(vector_to_sort[i++]);
        }
        //printf("size of buckets:%d, of alphabet:%d\n", sizeof(buckets), alphabet_size);
 
        auto id = 0;
#pragma omp parallel shared(id, alphabet_size)
        {
            while(id < alphabet_size){
                second_radix(buckets[id++], mapping_Function, alphabet_size, string_lengths, index+1);
            }
        }
 
        for (auto i = 0, shift = 0; i < alphabet_size; i++) {
 
            auto start = buckets[i].begin();
            auto vect_start = vector_to_sort.begin()+shift;
            auto difference = buckets[i].size();
            while(difference != 0){
                *vect_start++ = *start++;
                difference--;
            }
            shift += buckets[i].size();
        }
 
 
    }else{
        return;
    }
 
 
 
}
void radix_par(std::vector<std::string *> &vector_to_sort, const MappingFunction &mappingFunction,
               unsigned long alphabet_size, unsigned long string_lengths) {
    second_radix( vector_to_sort, mappingFunction, alphabet_size, string_lengths, 0);
 
   
}
