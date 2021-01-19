#ifndef PRODUCENTCONSUMER_THREADPOOL_H
#define PRODUCENTCONSUMER_THREADPOOL_H
 
#include <list>
#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>
 
template<typename job_t, typename worker_t>
class ThreadPool {
private:
    // Fronta uloh
    std::list<job_t> queue;
 
    // Vlakna konzumentu zpracovavajicich ulohy
    std::vector<std::thread> threads;
 
    // Funkce, kterou maji konzumenti vykonavat
    const worker_t & worker;
 
public:
    ThreadPool(const unsigned int num_threads, const worker_t & worker);
    void process(const job_t job);
    void join();
 
};
std::mutex lock;
std::condition_variable queue_wait;
 
template<typename job_t, typename worker_t>
 
 
ThreadPool<job_t, worker_t>::ThreadPool(const unsigned int num_threads, const worker_t &worker) : worker(worker) {
    
    for (unsigned int thread = 0; thread < num_threads; thread++) {
        threads.push_back(std::thread([&]() {
            job_t task;
            do {
                std::unique_lock<std::mutex>l(lock); // lock je unikatni a vaze se na mutex
                if(queue.empty())queue_wait.wait(l, [this] {return  !queue.empty();}); // viz. prednaska; cond. variable ceka na lock, dokud nebude neco v queue
                // Pote ji zpracujeme:
                task = queue.front();        // Precteme prvni ulohu ve fronte
                queue.pop_front();           // A ulohu odstranime
                l.unlock();
                if (task != 0) worker(task);  // Na zaver zavolame "worker" funkci, ktera ulohu vykona
 
            } while (task != 0);              // Pokud je "zpracovana" uloha 0, skoncime
        }));
 
    }
 
    
}
template<typename job_t, typename worker_t>
void ThreadPool<job_t, worker_t>::process(const job_t job) {
    // Bezpecne vlozte ulohu "job" do fronty uloh "queue"
    std::unique_lock<std::mutex>l(lock);
    // Pridat ulohu do fronty muzeme nasledujicim volanim:
    queue.push_back(job);
    l.unlock();
    queue_wait.notify_all();
    // Pokud timto zpusobem bude ale vice vlaken bude pristupovat ke fronte soucasne,
    // vysledek neni determinsticky. Mohlo by se nam napriklad stat, ze nam jine
    // vlakno nasi pridanou ulohu "prepise" a ta se tak ztrati.
    // Zabezpecte, ze k tomuto nemuze dojit.
}
 
// Tato metoda nam umozni volajici funkci v main.cpp pockat na vsechna spustena vlakna konzumentu
template<typename job_t, typename worker_t>
void ThreadPool<job_t, worker_t>::join() {
    for(unsigned int i = 0 ; i < threads.size() ; i++) threads[i].join();
}
 
#endif //PRODUCENTCONSUMER_THREADPOOL_H
