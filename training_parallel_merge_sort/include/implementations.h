//
// Created by myralllka on 10/7/21.
//

#pragma once

#include <vector>
#include <iostream>
#include <cstdlib>
#include <threads.h>

[[maybe_unused]] void sort_linear(std::vector<ssize_t> &input_vector);

[[maybe_unused]] void merge_sort(std::vector<ssize_t> &input_vector, ssize_t begin, ssize_t end);

[[maybe_unused]] void merge(std::vector<ssize_t> &input_vector, size_t begin, size_t end);

//###
[[maybe_unused]] std::vector<ssize_t> merge_sort_easiest(std::vector<ssize_t> in_vector);

[[maybe_unused]] std::vector<ssize_t> merge_easiest(std::vector<ssize_t> &in1, std::vector<ssize_t> &in2);
