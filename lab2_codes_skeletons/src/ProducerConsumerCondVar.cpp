#include <thread>
#include <vector>
#include <mutex>
#include <iostream>
#include <optional>
#include <queue>
#include <atomic>
#include <condition_variable>

std::condition_variable global_cv;
std::atomic<bool> finished = false;

void producer(size_t num_values_to_produce, std::queue<size_t> &q, std::mutex &q_mutex) {
//    std::srand(std::time(nullptr));
    for (size_t i = 0; i < num_values_to_produce; ++i) {
        std::unique_lock<std::mutex> unique_lock{q_mutex};
        q.push(i);
        unique_lock.unlock();
        global_cv.notify_all();
    }
    finished = true;
}

void consumer(size_t id, std::queue<size_t> &q, std::mutex &q_mutex) {
    // Perform the loop until all the values are consumed.

    while (not finished) {
        std::unique_lock<std::mutex> unique_lock{q_mutex};
        global_cv.wait(unique_lock, [&]() {
            return finished or !q.empty();
        });
        std::cout << q.front() << std::endl;
        q.pop();
        unique_lock.unlock();
    }
}

int main() {
    size_t num_consumers = 2;  // Number of consumers.
    size_t num_values_to_produce = 1000;   // Number of values to produce.
    std::mutex q_mutex;
    std::queue<size_t> q;
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
