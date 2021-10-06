// See CounterSecondTry.cpp from the C++11 threads lab part 1,
// where the same functionality is programmed using mutex and lock guards.

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

void increment_counter(std::atomic<size_t> &cntr) {
    for (size_t i = 0; i < 10000000; ++i) {
        ++cntr;
    }
}

int main() {
    std::atomic<size_t> counter = 0;

    std::vector<std::thread> workers;
    for (size_t worker_index = 0; worker_index < 8; ++worker_index) {
        workers.emplace_back(increment_counter, std::ref(counter));
    }

    for (auto &worker: workers) {
        worker.join();
    }

    std::cout << counter << std::endl;

    return 0;
}
