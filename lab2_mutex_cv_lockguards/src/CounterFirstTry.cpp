#include <iostream>
#include <thread>
#include <vector>

//using namespace std;

void incrementCounter(int *pCounter) {
    for (int i = 0; i < 10000000; i++) {
        (*pCounter)++;
    }
}

int main() {
    int counter = 0;

    std::vector<std::thread> workers;
    workers.reserve(4);
    for (int workerIndex = 0; workerIndex < 4; workerIndex++) {
        workers.emplace_back(incrementCounter, &counter);
    }

    for (auto &worker: workers) {
        worker.join();
    }

    std::cout << "Value of the counter variable: " << counter << std::endl;

    return 0;
}
