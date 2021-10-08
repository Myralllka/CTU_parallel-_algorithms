// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include "implementations.h"

int main() {

    std::vector<ssize_t> vector_to_sort{8, 7, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 8};
//    std::vector<ssize_t> vector_to_sort{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//    std::vector<ssize_t> vector_to_sort{ 11, 12, 14, 15, 16, 17, 1, 23, 45, 1, 0};

    sort_linear(vector_to_sort);
//    merge_easier()
//    auto res = merge_easiest(vector_to_sort, vector_to_sort2);
    for (auto &el: vector_to_sort) {
        std::cout << el << ", ";
    }
    return 0;
}
