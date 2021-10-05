#include <thread>
#include <vector>
#include <mutex>
#include <iostream>
#include <optional>
#include <queue>
#include <cstdint>


void producer(size_t num_values_to_produce, std::queue<int> &q, std::mutex &q_mutex) {
//    std::srand(std::time(nullptr));
    for (size_t i = 0; i < num_values_to_produce; ++i) {
        // Value to be consumed.
        std::lock_guard<std::mutex> l{q_mutex};
        q.push(std::rand());
//        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    }

    // TODO: some work?
}

void consumer(size_t id, std::queue<int> &q, std::mutex &q_mutex) {
    // Perform the loop until all the values are consumed.
    while (true) {
        std::lock_guard<std::mutex> l{q_mutex};
        if (q.empty()) return;
        q.pop();
        std::cout << id << std::endl;
    }
}

int main() {
    int num_consumers = 4;  // Number of consumers.
    int num_values_to_produce = 100;   // Number of values to produce.
    std::mutex q_mutex;
    std::queue<int> q;
    std::vector<std::thread> workers;
    workers.reserve(num_consumers);

    workers.emplace_back(producer, num_values_to_produce, std::ref(q), std::ref(q_mutex));

    for (int consumer_index = 0; consumer_index < num_consumers; consumer_index++) {
        workers.emplace_back(consumer, consumer_index, std::ref(q), std::ref(q_mutex));
    }


    for (auto &worker: workers) {
        worker.join();
    }

    std::cout << "All workers finished." << std::endl;
    std::cout << "Num remaining values queue: " << q.size() << std::endl;    // Check that all values were consumed.
}
