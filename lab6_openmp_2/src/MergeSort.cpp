#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

#include "Utils.hpp"

using namespace std;

void merge_sort_sequential(vector<double> &u) {
    // TODO: you can use inplace_merge() from <algorithm> for merging arrays.
}

void merge_sort_parallel(vector<double> &u) {
    // TODO: you can use inplace_merge() from <algorithm> for merging arrays.
}

int main() {
    vector<double> u = generate_random_vector(5000000);

    {
        auto u_copy = u;
        Stopwatch sw;
        sw.start();
        merge_sort_sequential(u_copy);
        sw.stop();
        cout << "Sequential merge sort: " << sw.duration().count()
             << " ms, sorted correctly " << is_sorted(u_copy.begin(), u_copy.end()) << endl;
    }

    {
        auto u_copy = u;
        Stopwatch sw;
        sw.start();
        merge_sort_parallel(u_copy);
        sw.stop();
        cout << "Parallel merge sort: " << sw.duration().count()
             << " ms, sorted correctly " << is_sorted(u_copy.begin(), u_copy.end()) << endl;
    }

    return 0;
}
