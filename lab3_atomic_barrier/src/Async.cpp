#include <iostream>
#include <future>

int mulltiply(size_t input1, size_t input2) {
    std::cout << "In multiply function, thread id: " << std::this_thread::get_id() << std::endl;
    return input1 * input2;
}

int main() {
    size_t input1 = 10;
    size_t input2 = 5;

    std::cout << "The main thread id " << std::this_thread::get_id() << std::endl;

    std::cout << "Async with launch::async" << std::endl;
    // TODO: create with async, get value

    std::cout << "Async with launch::deferred" << std::endl;
    // TODO: create with async, get value

    return 0;
}

