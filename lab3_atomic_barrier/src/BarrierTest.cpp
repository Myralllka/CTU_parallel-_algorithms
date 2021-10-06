#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>

#include "Barrier.hpp"

void barrier_waiting(size_t num_iterations, BarrierCV &barrier) {
    for (size_t i = 0; i < num_iterations; ++i) {}
    barrier.wait();
//    std::cout << "waiting" << std::endl;
//    std::cout << "done" << std::endl;
}

int main() {
    size_t num_repetitions = 2000;
    size_t num_iterations = 5000000;
    size_t num_workers = 10;

    BarrierCV barrier(num_workers);

    for (size_t i = 0; i < num_repetitions; ++i) {
        std::vector<std::thread> workers;
        workers.reserve(num_workers);

        for (size_t worker_index = 0; worker_index < num_workers; ++worker_index) {
            workers.emplace_back(barrier_waiting, num_iterations, std::ref(barrier));
        }

        for (auto &worker: workers) {
            worker.join();
        }
        std::cout << i << std::endl;
    }

    return 0;
}

