#include <iostream>
#include <vector>
#include <omp.h>


int main() {
    std::vector<int> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};


    #pragma omp parallel for default(none) shared(values)
    for (int & value : values) {

        value *= 10;
    }

    int x = 0;

    #pragma omp parallel for default(none) shared(values) reduction(+:x)
    for (int value : values) {
        x += value;
    }

    // Has to be 550.
    std::cout << "Value of x: " << x << std::endl;

    return 0;
}
