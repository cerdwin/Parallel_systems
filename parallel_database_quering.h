#define DATABASEQUERIES_QUERY_H
 
#include <vector>
#include <functional>
#include <atomic>
 
template<typename row_t>
using predicate_t = std::function<bool(const row_t &)>;
 
 
 
template<typename row_t>
bool is_satisfied_for_all(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table);
 
template<typename row_t>
bool is_satisfied_for_any(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table);
 
 
 
template<typename row_t>
bool is_satisfied_for_all(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table) {
    std::atomic<bool> solution { true };        // viz. decompose.cpp   // vektoru 'data_table'. Velikost vektoru tedy odpovida// poctu zaznamu
    #pragma omp parallel for
     for (unsigned int x = 0; x < predicates.size(); x++){
         std::atomic<bool> is_predicate_satisfied { false };
        for(unsigned int i = 0 ; i < data_table.size() ; i++) {
 
             if(predicates[x](data_table[i])) {
                is_predicate_satisfied = true;
                i+=data_table.size();
             }
         }
        if(is_predicate_satisfied == false){
            x+= predicates.size();
            solution = false;
   #pragma omp cancel for
        }
     }
    return solution;
}
 
template<typename row_t>
bool is_satisfied_for_any(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table) {
 
    std::atomic<bool> is_satisfied { false };
    unsigned int row_count = data_table.size();    // Kazdy radek tabulky je reprezentovany jednim prvkem
    #pragma omp parallel for
    for (unsigned int x = 0; x < row_count; x++){
        for(unsigned int i = 0 ; i < predicates.size() ; i++) {
            if(predicates[i](data_table[x])) {
                is_satisfied = true;
            }
        }
        if(is_satisfied){
            x+= row_count;
        #pragma omp cancel for
        }
    }
    return is_satisfied;
}
#endif //DATABASEQUERIES_QUERY_H
