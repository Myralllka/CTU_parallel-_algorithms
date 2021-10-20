#include <random>
#include <algorithm>
#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

// Do in parallel.
[[maybe_unused]] size_t fn_num_samples_in_circle(size_t random_seed, size_t num_samples) {
    std::mt19937 random_engine(random_seed);

    std::uniform_real_distribution<float> dist_x(-1.0, 1.0);
    std::uniform_real_distribution<float> dist_y(-1.0, 1.0);

    size_t num_samples_in_circle = 0;
    double x, y, distance;

    for (size_t i = 0; i < num_samples; ++i) {
        x = dist_x(random_engine);
        y = dist_y(random_engine);
        distance = (x * x + y * y);
        if (distance <= 1) {
            ++num_samples_in_circle;
        }
    }

    return num_samples_in_circle;
}

int main() {
    size_t total_num_samples = 100000000;
    size_t num_iterations = 10000;
    size_t num_samples_per_iteration = total_num_samples / num_iterations;

    if ((total_num_samples % num_iterations) != 0) {
        std::cout << "Total number of samples must be multiple of number of iterations!" << std::endl;
        exit(1);
    }

    size_t num_samples_in_circle = 0;
#pragma omp parallel for default(none) firstprivate(num_samples_per_iteration) reduction(+:num_samples_in_circle)
    for (size_t i = 0; i < num_samples_per_iteration; ++i) {
        num_samples_in_circle += fn_num_samples_in_circle(i, num_samples_per_iteration);
    }
    // TODO: compute num_samples_in_circle in parallel
//    auto res = fn_num_samples_in_circle(1, 10);

    std::cout <<
              (4.0 * static_cast<double>(num_samples_in_circle)) / (1.0 * static_cast<double>(total_num_samples)) <<
              std::endl;
    return 0;
}
