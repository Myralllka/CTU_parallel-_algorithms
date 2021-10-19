#include <random>
#include <algorithm>
#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

// Do in parallel.
[[maybe_unused]] size_t fn_num_samples_in_circle(size_t random_seed, size_t num_samples) {
    std::mt19937 randomEngine(random_seed);
    std::uniform_real_distribution<float> dist(-1.0, 1.0);

    int num_samples_in_circle = 0;
    // TODO:

    return num_samples_in_circle;
}

int main() {
    size_t total_num_samples = 1000000000;

    size_t num_iterations = 100;
    size_t num_samples_per_iteration = total_num_samples / num_iterations;
    if ((total_num_samples % num_iterations) != 0) {
        std::cout << "Total number of samples must be multiple of number of iterations!" << std::endl;
        exit(1);
    }

    size_t num_samples_in_circle = 0;

    // TODO: compute num_samples_in_circle in parallel

    std::cout << (4 * num_samples_in_circle) / (1 * total_num_samples) << std::endl;
    return 0;
}
