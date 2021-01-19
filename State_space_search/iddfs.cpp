#include <queue>
#include <algorithm>
#include <unordered_set>
#include <limits>
#include <atomic>
#include <omp.h>
#include <iostream>
using namespace std;
#include "iddfs.h"
 
 
 
////tady zacina pouzita cast
int method = 0;
//std::unordered_map<unsigned long, unsigned int> visited;
std::shared_ptr<const state> dive_dfs(int depth, std::shared_ptr<const state> root);
std::shared_ptr<const state> iddfs(std::shared_ptr<const state> root) {
 
    if(root->is_goal())return root;
 
    //std::vector<std::shared_ptr<const state>> start_states; //= new std::vector<std::shared_ptr<const state>>;
    std::queue<std::shared_ptr<const state>> start_states;
    std::vector<std::shared_ptr<const state>> beginning_states;
    std::shared_ptr<const state> analysed_predecessor = root;
    std::unordered_set<unsigned long long >states;
    std::shared_ptr<const state> best_result = nullptr;
    start_states.push(root);
    std::shared_ptr<const state> top;
    while(start_states.size() < 8 ) { // I fill my vector of at least 8 states from the beginning of the grid
        top = start_states.front();
        start_states.pop();
        std::vector<std::shared_ptr<const state>> nearby = top->next_states();
        // first we make our first population
        for(int i = 0; i < nearby.size(); i++){
            std::shared_ptr<const state> filler;
            filler = nearby[i];
            if(states.find(filler->get_identifier()) == states.end()){
                if(filler->is_goal() && best_result == nullptr){
                    best_result = filler;
                }else if(filler->is_goal() && best_result != nullptr && filler->current_cost() < best_result->current_cost()){
                    best_result = filler;
                }else if(filler->is_goal() && best_result != nullptr && filler->current_cost() == best_result->current_cost() && filler->get_identifier() < best_result->get_identifier()){
                    best_result = filler;
                }
                start_states.push(filler);
                states.insert(filler->get_identifier());
            }
        }
    }
 
    beginning_states.clear();
    if(best_result != nullptr){
        //printf("ending before parallelisation\n");
        return best_result;
    }
    int i = 0;
    while(!start_states.empty()){
        beginning_states.push_back(start_states.front());
        start_states.pop();
        i++;
    }
    int depth = 1;
    while(best_result == nullptr ){
#pragma omp parallel for
        for(int x = 0; x < beginning_states.size(); x++) {
 
            //if(my_depth <= 0)continue;
            //if(best_result != nullptr)continue;
 
            unsigned long long my_depth = depth - beginning_states[x]->current_cost();
            std::shared_ptr<const state>my_result = dive_dfs(depth, beginning_states[x]);//dive_dfs(depth, beginning_states[x]);
            //fprintf(stdout, "entering critical section to check my results with global optima\n");
#pragma omp critical
            if(my_result!= nullptr && my_result->is_goal()){
                if(best_result == nullptr|| my_result->current_cost()<best_result->current_cost()){
                    best_result = my_result;
                }else if(best_result->get_identifier()>my_result->get_identifier() && my_result->current_cost()<=best_result->current_cost() ){
                    best_result = my_result;
                }
            }
            //fprintf(stdout, "checked out of the crit. section\n");
 
        }
        //printf("method is:%d\n", method);
    depth++;
    }
    return best_result;
}
 
 
/////working
std::shared_ptr<const state> dive_dfs(int depth, std::shared_ptr<const state> root){
    //fprintf(stdout, "1.entering dfs, now I will check if root is nullptr\n");
    if(root == nullptr )return nullptr;
    //fprintf(stdout, "2. in dfs, now I will check if root is goal\n");
    if(root->is_goal()){
        //fprintf(stdout, "2.2.found goal, :%llu\n", root->get_identifier());
        return root;
    }
    //fprintf(stdout, "3. in dfs, now I will check if depth is zero \n");
    if(depth == 0){
        //fprintf(stdout, "Were in dfs and depth is zero\n");
        return nullptr;
    }
 
    std::shared_ptr<const state> outcome = nullptr;
    if(root->is_goal())outcome = root;
    //fprintf(stdout, "4. in dfs, we are here... \n");
    if(root != nullptr){
 
        std::vector<std::shared_ptr<const state>> neighbours = root->next_states();
        //fprintf(stdout, "4. in dfs, we are in if... \n");
//#pragma omp parallel for
        //fprintf(stdout, "entering for loop to check for neighbours\n");
        for(int i = 0; i< neighbours.size(); i++){
            if(root->get_predecessor()->get_identifier() == neighbours[i]->get_identifier())continue;
            std::shared_ptr<const state> descendant = dive_dfs(depth-1, neighbours[i]);
            //if(descendant != nullptr)return descendant;
            if(descendant != nullptr && outcome == nullptr)outcome = descendant;
            if(descendant != nullptr && outcome != nullptr){
                //fprintf(stdout, "I maybe found a better result\n");
                if(descendant->current_cost()< outcome->current_cost())outcome = descendant;
                else if(descendant->current_cost() == outcome->current_cost() && descendant->get_identifier()< outcome->get_identifier())outcome = descendant;
            }
        }
 
    }
 
    return outcome;
}
 
////////tady konci pouzita c ast
