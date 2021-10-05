#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

void increment_counter(int &counter, std::mutex &m) {
    for (int i = 0; i < 10000000; ++i) {
        std::lock_guard<std::mutex> l{m};
        counter++;
    }
}

int main() {
    int counter = 0;
    std::mutex counter_mutex;
    std::vector<std::thread> workers;
    workers.reserve(4);
    for (int worker_index = 0; worker_index < 4; ++worker_index) {
        workers.emplace_back(increment_counter, std::ref(counter), std::ref(counter_mutex));
    }

    for (auto &worker: workers) {
        worker.join();
    }

    std::cout << "Value of the counter variable: " << counter << std::endl;

    return 0;
}
