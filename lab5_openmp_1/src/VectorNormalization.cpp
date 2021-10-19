#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

#include "Utils.hpp"

double compute_vector_length(const std::vector<double> &u) {
    double sum_squares = 0;
    for (auto &i: u) {
        sum_squares += i * i;
    }

    return sqrt(sum_squares);
}

std::vector<double> normalization_sequential(std::vector<double> u) {
    // TODO: work on vector u inplace
    return u;
}

std::vector<double> normalization_parallel_sections(std::vector<double> u) {
    // TODO: work on vector u inplace
    return u;
}

std::vector<double> normalization_parallel_for_and_critical(std::vector<double> u) {
    // TODO: work on vector u inplace
    return u;
}

std::vector<double> normalization_parallel_for_and_reduction(std::vector<double> u) {
    // TODO: work on vector u inplace
    return u;
}

int main() {
    std::vector<double> u = generate_random_vector(50000000);

    std::cout << "Length of the input vector: " << compute_vector_length(u) << std::endl;

    {
        Stopwatch sw;
        sw.start();
        auto u_norm = normalization_sequential(u);
        sw.stop();
        std::cout << "Sequential: " << sw.duration().count() << " ms, length " << compute_vector_length(u_norm)
                  << std::endl;
    }

    {
        Stopwatch sw;
        sw.start();
        auto u_norm = normalization_parallel_sections(u);
        sw.stop();
        std::cout << "Parallel sections: " << sw.duration().count() << " ms, length " << compute_vector_length(u_norm)
                  << std::endl;
    }

    {
        Stopwatch sw;
        sw.start();
        auto u_norm = normalization_parallel_for_and_critical(u);
        sw.stop();
        std::cout << "Parallel for + critical section: " << sw.duration().count() << " ms, length "
                  << compute_vector_length(
                          u_norm) << std::endl;
    }

    {
        Stopwatch sw;
        sw.start();
        auto u_norm = normalization_parallel_for_and_reduction(u);
        sw.stop();
        std::cout << "Parallel for + reduction: " << sw.duration().count() << " ms, length " << compute_vector_length(
                u_norm) << std::endl;
    }

    return 0;
}
