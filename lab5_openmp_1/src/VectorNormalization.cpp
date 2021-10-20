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

std::vector<double> normalization_sequential(std::vector<double> vector) {
    auto length = compute_vector_length(vector);
    for (double &i: vector) {
        i /= length;
    }
    return vector;
}

std::vector<double> normalization_parallel_sections(std::vector<double> vector) {
    auto length = compute_vector_length(vector);
    size_t mid = vector.size() / 2;
#pragma omp parallel default(none) shared(vector, length, mid)
    {
#pragma omp sections
        {
#pragma  omp section
            {
                for (size_t i = 0; i < mid; ++i) {
                    vector[i] /= length;
                }
            }
#pragma  omp section
            {
                for (size_t i = mid; i < vector.size(); ++i) {
                    vector[i] /= length;
                }
            }
        }
    }
    return vector;
}

std::vector<double> normalization_parallel_for_and_critical(std::vector<double> vector) {
    double length = 0;

    for (auto &el: vector) {
        length += (el * el);
    }
    length = sqrt(length);

#pragma omp parallel for default(none) shared(vector) firstprivate(length)
    for (double &i: vector) {
        i /= length;
    }
    return vector;
}

std::vector<double> normalization_parallel_for_and_reduction(std::vector<double> vector) {
    double length = 0;
#pragma omp parallel for default(none) shared(vector) reduction(+:length)
    for (auto &el: vector) {
        length += (el * el);
    }
    length = sqrt(length);
#pragma omp parallel for default(none) shared(vector, length)
    for (double &i: vector) {
        i /= length;
    }
    return vector;
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
