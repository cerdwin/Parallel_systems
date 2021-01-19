#include "bfs.h"
#include<queue>
#include <iostream>
#include <vector>
#include <list>
#include <atomic>
#include <unordered_set>
#include <string.h>
#include <algorithm>
#include <map>
#include <omp.h>
#include <string>
using namespace std;
// Naimplementujte efektivni algoritmus pro nalezeni nejkratsi cesty v grafu.
// V teto metode nemusite prilis optimalizovat pametove naroky, a vhodnym algo-
// ritmem tak muze byt napriklad pouziti prohledavani do sirky (breadth-first
// search.
//
// Metoda ma za ukol vratit ukazatel na cilovy stav, ktery je dosazitelny pomoci
// nejkratsi cesty. Pokud je nejkratsich cest vice, vratte ukazatel na stav s nej-
// nizsim identifikatorem (viz metoda 'state::get_identifier()' v 'state.h').
 
std::shared_ptr<const state> bfs(std::shared_ptr<const state> root) {
    std::queue<std::shared_ptr<const state>> *ProcessingQueue = new std::queue<std::shared_ptr<const state>>; // queue where we keep all our processes to process
    unordered_set<unsigned long long >Visited; // where we keep track of all IDs we have popped from a queue
    int unfound = 1; // true, since we havent found a solution
    long long bestcost;
    std::shared_ptr<const state> beststate = nullptr;
    std::atomic<bool> ending(false);
    std::atomic<unsigned long long > queue_size;
    std::atomic<unsigned long long >j;
    if(root->is_goal()){
        bestcost = root->current_cost();
        beststate = root;
        unfound = 0;
    }
    std::vector<std::shared_ptr<const state>>first_gen;
    first_gen = root->next_states();
    Visited.insert(root->get_identifier());
    for(auto first:first_gen){
        if(first->is_goal())return first;
        Visited.insert(first->get_identifier());
        ProcessingQueue->push(first);
    }
    ProcessingQueue->push(root);
    while(!ProcessingQueue->empty() ){
        int sizeofqueue = ProcessingQueue->size();
        #pragma omp parallel for
        for(int i = 0; i< sizeofqueue; i++){
            std::shared_ptr<const state> currentnode;
            std::vector<std::shared_ptr<const state>> neighbours;
            #pragma omp critical
            {
                currentnode = ProcessingQueue->front();// our current node is at the front of the queue
                ProcessingQueue->pop(); // we remove it from the queue to avoid repetition
 
            }
            neighbours = currentnode->next_states();
            int neighbour_size = neighbours.size();
            for (int i = 0; i< neighbour_size; i++) {
                std::shared_ptr<const state>temp = neighbours[i];
                if (temp->is_goal()) {
                    if(temp->current_cost()<bestcost || unfound==1){
                        beststate = temp;
                        bestcost = temp->current_cost();
                    }else if(temp->current_cost() == bestcost && temp->get_identifier()<beststate->get_identifier()){
                        beststate = temp;
                    }
                    unfound = 0;
                } else if(unfound ==1 || temp->current_cost()<bestcost){
#pragma omp critical
                    if(Visited.count(temp->get_identifier()) == 0) {
                        ProcessingQueue->push(temp);
                        Visited.insert(temp->get_identifier());
                    }
                }
            }
 
        }
 
        }
    //printf("were out\n");
 
    return beststate;
