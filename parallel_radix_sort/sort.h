#define CODE_SORT_H
 
#include <vector>
#include <algorithm>
#include <iostream>
 
template<typename element_t>
using SortingAlgorithm = void (*)(std::vector<element_t *> &vector_to_sort);

using MappingFunction = const unsigned long (*)(const char &character_of_alphabet);
 

void radix_par(std::vector<std::string *> &vector_to_sort, const MappingFunction &mappingFunction,
               unsigned long alphabet_size, unsigned long string_lengths);
 
#endif //CODE_SORT_H
