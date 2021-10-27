#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <algorithm>
#include "Utils.hpp"


void merge_sort(std::vector<double> &input_vector, ssize_t begin, ssize_t end) {
    if (end < begin) exit(1);
    if (end - begin == 0) return;
    if (end - begin == 1) {
        if (input_vector[begin] > input_vector[end]) {
            std::swap(input_vector[begin], input_vector[end]);
        }
        return;
    }
    ssize_t middle = (end + begin) / 2;
    merge_sort(input_vector, begin, middle);
    merge_sort(input_vector, middle, end);
    std::inplace_merge(input_vector.begin() + begin, input_vector.begin() + middle, input_vector.begin() + end);
}

void merge_sort_parallel_impl(std::vector<double> &input_vector, ssize_t begin, ssize_t end) {
    if (end < begin) exit(1);
    if (end - begin == 0) return;
    ssize_t middle = (end + begin) / 2;

    if (end - begin < 32) {

        merge_sort(input_vector, begin, end);

    } else {
#pragma omp taskgroup
        {
#pragma omp task default(none) shared(input_vector) firstprivate(begin, middle, end)
            merge_sort_parallel_impl(input_vector, begin, middle);
#pragma omp task default(none) shared(input_vector) firstprivate(begin, middle, end)
            merge_sort_parallel_impl(input_vector, middle, end);
        }

        std::inplace_merge(input_vector.begin() + begin, input_vector.begin() + middle, input_vector.begin() + end);
    }
}

void merge_sort_sequential(std::vector<double> &u) {
    merge_sort(u, 0, u.size());
}

void merge_sort_parallel(std::vector<double> &u) {
#pragma omp parallel default(none) shared(u)
    {
#pragma omp single
        {
            merge_sort_parallel_impl(u, 0, u.size());
        }
    }

}

int main() {
    std::vector<double> u = generate_random_vector(500000);

    {
        auto u_copy = u;
        Stopwatch sw;
        sw.start();
        merge_sort_sequential(u_copy);
        sw.stop();
        std::cout << "Sequential merge sort: " << sw.duration().count()
                  << " ms, sorted correctly " << is_sorted(u_copy.begin(), u_copy.end()) << std::endl;
    }

    {
        auto u_copy = u;
        Stopwatch sw;
        sw.start();
        merge_sort_parallel(u_copy);
        sw.stop();
        std::cout << "Parallel merge sort: " << sw.duration().count()
                  << " ms, sorted correctly " << is_sorted(u_copy.begin(), u_copy.end()) << std::endl;
    }

    return 0;
}
