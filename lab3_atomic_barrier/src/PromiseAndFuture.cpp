#include <iostream>
#include <thread>
#include <future>

void multiply(std::promise<size_t> in1, std::promise<size_t> in2, std::promise<size_t> &out) {
    size_t in1v = in1.get_future().get();
    size_t in2v = in2.get_future().get();
    out.set_value(in1v * in2v);
}

int main() {
    std::promise<size_t> in1;
    std::promise<size_t> in2;
    std::promise<size_t> out;
    in1.set_value(2);
    in2.set_value(4);

    std::thread worker(multiply, std::move(in1), std::move(in2), std::ref(out));

    std::cout << "Output obtained in main thread: " << out.get_future().get() << std::endl;

    worker.join();

    return 0;
}
