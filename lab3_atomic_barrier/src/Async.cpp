#include <iostream>
#include <future>
#include <thread>


size_t mulltiply(size_t input1, size_t input2) {
    std::cout << "In multiply function, thread id: " << std::this_thread::get_id() << std::endl;
    return input1 * input2;
}

int main() {

    std::cout << "The main thread id " << std::this_thread::get_id() << std::endl;

    std::cout << "Async with launch::async" << std::endl;

    std::future<size_t> eager = std::async(std::launch::async, mulltiply, 7, 8);

    std::cout << eager.get() << std::endl;

    std::cout << "Async with launch::deferred" << std::endl;

    std::future<size_t> lazy = std::async(std::launch::deferred, mulltiply, 5, 6);

    std::cout << lazy.get() << std::endl;

    return 0;
}

