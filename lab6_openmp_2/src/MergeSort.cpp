#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

#include "Utils.hpp"

void merge_sort_sequential(vector<double> &u) {
    // TODO: you can use inplace_merge() from <algorithm> for merging arrays.
}

void merge_sort_parallel(vector<double> &u) {
    // TODO: you can use inplace_merge() from <algorithm> for merging arrays.
}

int main() {
    std::vector<double> u = generate_random_vector(5000000);

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
