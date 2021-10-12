// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
//
// Created by myralllka on 10/7/21.
//

#include "implementations.h"


void sort_linear(std::vector<ssize_t> &input_vector) {
    quick_sort(input_vector, 0, input_vector.size() - 1);
}

ssize_t partition(std::vector<ssize_t> &values, ssize_t left, ssize_t right) {
    ssize_t pivot_index = left + (right - left) / 2;
    ssize_t pivot_value = values[pivot_index];
    ssize_t i = left, j = right;
    ssize_t temp;
    while (i <= j) {
        while (values[i] < pivot_value) ++i;
        while (values[j] > pivot_value) --j;
        if (i <= j) {
            temp = values[i];
            values[i] = values[j];
            values[j] = temp;
            ++i;
            --j;
        }
    }
    return i;
}

void quick_sort(std::vector<ssize_t> &values, ssize_t left, ssize_t right) {
    if (left < right) {
        ssize_t pivot_index = partition(values, left, right);
        quick_sort(values, left, pivot_index - 1);
        quick_sort(values, pivot_index, right);
    }
}


void merge_sort(std::vector<ssize_t> &input_vector, ssize_t begin, ssize_t end) {
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
    merge(input_vector, begin, end);
}

void merge(std::vector<ssize_t> &input_vector, size_t begin, size_t end) {

    if (end < begin) exit(2);

    if (end - begin < 2) exit(3);

    std::vector<ssize_t> local_result{};
    local_result.reserve(end - begin);

    size_t middle = (end + begin) / 2;
    size_t i = begin;
    size_t j = middle;

    while (i < middle and j < end) {
        if (input_vector[i] < input_vector[j])
            local_result.push_back(input_vector[i++]);
        else
            local_result.push_back(input_vector[j++]);

    }

    while (i < middle) local_result.push_back(input_vector[i++]);
    while (j < end) local_result.push_back(input_vector[j++]);


    for (size_t index = 0; index < (end - begin); ++index) {
        input_vector[index + begin] = local_result[index];
    }
}

std::vector<ssize_t> merge_easiest(std::vector<ssize_t> &in1, std::vector<ssize_t> &in2) {
    std::vector<ssize_t> local_result{};
    local_result.reserve(in1.size() + in2.size());
    size_t i = 0, j = 0;
    while (i < in1.size() and j < in2.size()) {
        [[maybe_unused]] auto a = in1.size();
        [[maybe_unused]] auto a2 = in2.size();
        if (in1[i] > in2[j]) {
            local_result.push_back(in2[j]);
            ++j;
        } else {
            local_result.push_back(in1[i]);
            ++i;
        }
    }
    while (i < in1.size()) local_result.push_back(in1[i++]);
    while (j < in2.size()) local_result.push_back(in2[j++]);
    return local_result;
}

std::vector<ssize_t> merge_sort_easiest(std::vector<ssize_t> in_vector) {
    if (in_vector.size() <= 2) {
        if (in_vector.size() == 1) return in_vector;
        else {
            if (in_vector[0] > in_vector[1]) {
                return std::vector<ssize_t>{in_vector[1], in_vector[0]};
            }
            return in_vector;
        }
    } else {
        size_t middle = in_vector.size() / 2;
        auto vec1 = std::vector<ssize_t>(in_vector.begin(), in_vector.begin() + static_cast<ssize_t>(middle));
        auto vec2 = std::vector<ssize_t>(in_vector.begin() + static_cast<ssize_t>(middle), in_vector.end());

        vec1 = merge_sort_easiest(vec1);
        vec2 = merge_sort_easiest(vec2);
        auto res = merge_easiest(vec1, vec2);
        return res;
    }
}
