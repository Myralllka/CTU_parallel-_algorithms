#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>

#include "Barrier.hpp"

void barrierWaiting(size_t numIterations, Barrier &barrier) {
    for (size_t i = 0; i < numIterations; ++i) {
        barrier.wait();
    }
}

int main() {
    size_t numRepetitions = 5;
    size_t numIterations = 50;
    size_t numWorkers = 10;

    Barrier barrier(numWorkers);

    for (size_t i = 0; i < numRepetitions; ++i) {
        std::vector<std::thread> workers;
        workers.reserve(numWorkers);
        for (size_t workerIndex = 0; workerIndex < numWorkers; ++workerIndex) {
            workers.emplace_back(barrierWaiting, numIterations, std::ref(barrier));
        }

        for (auto &worker: workers) {
            worker.join();
        }
    }

    return 0;
}

