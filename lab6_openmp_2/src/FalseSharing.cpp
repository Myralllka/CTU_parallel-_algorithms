#include <iostream>
#include <vector>
#include <omp.h>

#include "Utils.hpp"

double with_false_sharing(int num_iterations) {
    double x = 0.0;
    double y = 0.0;
#pragma omp parallel num_threads(2) default(none) shared(x, y, num_iterations)
    {
#pragma omp sections
        {
#pragma omp section
            {
                for (int i = 0; i < num_iterations; i++) {
                    x += sin(i) * cos(i);
                }
            }

#pragma omp section
            {
                for (int i = 0; i < num_iterations; i++) {
                    y += sin(i) * cos(i);
                }
            }
        }
    }

    return x + y;
}

double no_false_sharing(int num_iterations) {
    double x = 0.0;
    double y = 0.0;
#pragma omp parallel num_threads(2) default(none) shared(num_iterations, x, y)
    {
#pragma omp sections
        {
#pragma omp section
            {
                double private_result = 0.0;
                for (int i = 0; i < num_iterations; i++) {
                    private_result += sin(i) * cos(i);
                }
                x = private_result;
            }

#pragma omp section
            {
                double private_result = 0.0;
                for (int i = 0; i < num_iterations; i++) {
                    private_result += sin(i) * cos(i);
                }
                y = private_result;
            }
        }
    }

    return x + y;
}

int main() {
    int num_iterations = 50000000;

    {
        Stopwatch sw;
        sw.start();
        auto result = with_false_sharing(num_iterations);
        sw.stop();
        std::cout << "with_false_sharing: " << sw.duration().count() << " ms, result " << result << std::endl;
    }

    {
        Stopwatch sw;
        sw.start();
        auto result = no_false_sharing(num_iterations);
        sw.stop();
        std::cout << "no_false_sharing: " << sw.duration().count() << " ms, result " << result << std::endl;
    }

    return 0;
}
