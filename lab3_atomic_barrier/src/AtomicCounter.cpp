// See CounterSecondTry.cpp from the C++11 threads lab part 1,
// where the same functionality is programmed using mutex and lock guards.

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

void incrementCounter(/* TODO: args*/) {
    for (size_t i = 0; i < 10000000; ++i) {
        // TODO
    }
}

int main() {
    // TODO: create atomic var

    std::vector<std::thread> workers;
    for (size_t workerIndex = 0; workerIndex < 4; ++workerIndex) {
        // TODO: create worker
    }

    for (auto &worker: workers) {
        worker.join();
    }

    // TODO: output atomic var

    return 0;
}
