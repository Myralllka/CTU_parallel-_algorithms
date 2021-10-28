//
// Created by myralllka on 10/25/21.
//

#ifndef LU_DECOMPOSITION_LU_H
#define LU_DECOMPOSITION_LU_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "barrier.hpp"

class LU {
private:
    size_t m_num_threads = std::thread::hardware_concurrency();
//    size_t m_num_threads = 2;

    std::vector<std::vector<double>> m_A;
    std::vector<std::vector<double>> m_L;
    std::vector<std::vector<double>> m_U;

    size_t m_size{};
    size_t m_k{};

    barrier m_barrier_update_A{m_num_threads};
    barrier m_barrier_update_ready_A{m_num_threads};

    friend std::ostream &operator<<(std::ostream &, const LU &);

public:
    LU() = default;

    ~LU() = default;

    void read_matrix_from_input_file(const std::string &input_file);

    void write_results_to_output_file(const std::string &output_file);

    [[maybe_unused]] void decompose_linear();

    void decompose();

    [[maybe_unused]] void parallel_update_A(size_t idx);

    [[maybe_unused]] void decompose_parallel();
};

#endif //LU_DECOMPOSITION_LU_H
