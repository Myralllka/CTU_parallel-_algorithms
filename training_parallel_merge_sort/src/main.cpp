// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
//
#include <iostream>
#include <vector>
#include "implementations.h"

// Driver program to test above functions
int main() {
    std::vector<ssize_t> arr{8, 7, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 8};
//    std::vector<int> arr = { 8, 7, 6, 5, 4, 3, 2, 1};
    sort_linear(arr);
    printf("Sorted array: \n");
    for (auto &el: arr) {
        std::cout << el << ", ";
    }
}
