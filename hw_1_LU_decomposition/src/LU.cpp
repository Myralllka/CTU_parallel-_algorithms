// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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
//#define DEBUG
//#define LINEAR
#define TRANSPOSE_L


class barrier {
private:
    std::mutex m_m;
    std::condition_variable m_cv;
    size_t counter;
    size_t waiting;
    size_t thread_count;

public:
    explicit barrier(size_t count) : thread_count(count), counter(0), waiting(0) {}

    void wait() {
        //fence mechanism
        std::unique_lock<std::mutex> lk(m_m);
        ++counter;
        ++waiting;
        m_cv.wait(lk, [&] { return counter >= thread_count; });
        m_cv.notify_one();
        --waiting;
        if (waiting == 0) {
#ifdef DEBUG
            std::cout << "[barrier] done" << std::endl;
#endif
            counter = 0;
        }
        lk.unlock();
    }
};

class LU {
private:
//    size_t m_num_threads = 4;
    size_t m_num_threads = std::thread::hardware_concurrency();

    std::vector<std::vector<double>> m_A;
    std::vector<std::vector<double>> m_L;
    std::vector<std::vector<double>> m_U;
#ifdef TRANSPOSE_L
    std::vector<std::vector<double>> m_L_T;
#endif
    std::vector<size_t> m_threads_indexes;

    size_t m_size{};
    size_t m_k{};

    barrier m_barrier_update_A{m_num_threads + 1};
    barrier m_barrier_update_ready_A{m_num_threads + 1};


    friend std::ostream &operator<<(std::ostream &, const LU &);

public:
    LU() {
        m_threads_indexes.resize(m_num_threads + 1, 0);
        m_threads_indexes.resize(m_num_threads + 1, 0);
    };

    ~LU() = default;

    void read_matrix_from_input_file(const std::string &input_file) {
        std::ifstream bin(input_file.c_str(), std::ifstream::in | std::ifstream::binary);
        if (bin.fail()) {
            throw std::invalid_argument("Cannot open the input file!");
        }

        size_t n = 0;
        bin.read((char *) &n, sizeof(size_t));
        m_A.resize(n, std::vector<double>(n, 0.0));
#ifdef TRANSPOSE_L
        m_L_T = m_L = m_U = m_A;
#else
        m_L = m_U = m_A;
#endif
        for (size_t r = 0; r < n; ++r) {
            bin.read((char *) m_A[r].data(), n * sizeof(double));
        }
        m_size = m_A.size();
    }

    void write_results_to_output_file(const std::string &output_file) {
        std::ofstream bout(output_file.c_str(), std::ofstream::out | std::ofstream::binary);
        if (bout.fail()) {
            throw std::invalid_argument("Cannot open the output file!");
        }
#ifdef TRANSPOSE_L
        for (size_t i = 0; i < m_size; ++i) {
            for (size_t j = 0; j < m_size; ++j) {
                m_L_T[i][j] = m_L[j][i];
            }
        }
#endif
        for (size_t r = 0; r < m_size; ++r) {
#ifdef TRANSPOSE_L
            bout.write((char *) m_L_T[r].data(), m_size * sizeof(double));
#else
            bout.write((char *) m_L[r].data(), m_size * sizeof(double));
#endif
        }
        for (size_t r = 0; r < m_size; ++r) {
            bout.write((char *) m_U[r].data(), m_size * sizeof(double));
        }

    }

    [[maybe_unused]] void decompose_linear() {
        for (m_k = 0; m_k < m_size; ++m_k) {
            for (size_t j = m_k; j < m_size; ++j) {
                m_U[m_k][j] = m_A[m_k][j];
            }
            m_L[m_k][m_k] = 1;
            for (size_t i = m_k + 1; i < m_size; ++i) {
#ifdef TRANSPOSE_L
                m_L[m_k][i] = m_A[i][m_k] / m_U[m_k][m_k];
#else
                m_L[i][m_k] = m_A[i][m_k] / m_U[m_k][m_k];
#endif
            }

            for (size_t i = m_k + 1; i < m_size; ++i) {
                for (size_t j = m_k + 1; j < m_size; ++j) {
#ifdef TRANSPOSE_L
                    m_A[i][j] = m_A[i][j] - m_L[m_k][i] * m_U[m_k][j];
#else
                    m_A[i][j] = m_A[i][j] - m_L[i][m_k] * m_U[m_k][j];
#endif
                }
            }
        }
    }

    void decompose() {
        if (m_num_threads * 2 >= m_A.size()) {
            decompose_linear();
        } else {
            decompose_parallel();
        }
    }

    [[maybe_unused]] void parallel_update_A(size_t idx) {
        for (size_t cnt = 0; cnt < m_size; ++cnt) {
            m_barrier_update_ready_A.wait();

            for (size_t i = m_threads_indexes[idx]; i < m_threads_indexes[idx + 1]; ++i) {
                for (size_t j = 0; j < m_size; ++j) {
#ifdef TRANSPOSE_L
                    m_A[i][j] = m_A[i][j] - m_L[m_k][i] * m_U[m_k][j];
#else
                    m_A[i][j] = m_A[i][j] - m_L[i][m_k] * m_U[m_k][j];
#endif
                }
            }
            m_barrier_update_A.wait();
        }
    }

    [[maybe_unused]] void decompose_parallel() {
        m_threads_indexes[m_num_threads] = m_size;
        std::vector<std::thread> vector_of_threads;

        for (size_t i = 0; i < m_num_threads; ++i) {
            vector_of_threads.emplace_back(&LU::parallel_update_A, this, i);
        }

        for (m_k = 0; m_k < m_size; ++m_k) {
#ifdef DEBUG
            std::cout << "decompose started" << m_k << std::endl;
#endif
            for (size_t j = m_k; j < m_size; ++j) {
                m_U[m_k][j] = m_A[m_k][j];
            }
            m_L[m_k][m_k] = 1;
            for (size_t i = m_k + 1; i < m_size; ++i) {
#ifdef TRANSPOSE_L
                m_L[m_k][i] = m_A[i][m_k] / m_U[m_k][m_k];
#else
                m_L[i][m_k] = m_A[i][m_k] / m_U[m_k][m_k];
#endif
            }
            {
                auto tmp_size = (m_size - (m_k + 1));
                auto step = tmp_size / (m_num_threads);

                for (size_t counter = 0; counter < m_num_threads; ++counter) {
                    m_threads_indexes[counter] = m_k + 1 + counter * step;
                }
                m_barrier_update_ready_A.wait();
                m_barrier_update_A.wait();
            }
        }

        for (auto &t: vector_of_threads) {
#ifdef DEBUG
            std::cout << ".";
#endif
            t.join();
        }
    }
};

// Print the matrices A, L, and U of an LU class instance.
std::ostream &operator<<(std::ostream &out, const LU &lu) {

    std::function<void(const std::vector<std::vector<double>> &)> print_matrix = [&](
            const std::vector<std::vector<double>> &M) {

        size_t n = M.size();
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                out << " " << std::setw(10) << M[i][j];
            }
            out << std::endl;
        }
    };

    std::function<void(const std::vector<std::vector<double>> &)> print_matrix_L = [&](
            const std::vector<std::vector<double>> &M) {

        size_t n = M.size();
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                out << " " << std::setw(10) << M[j][i];
            }
            out << std::endl;
        }
    };

    out << "Matrix A:" << std::endl;
    print_matrix(lu.m_A);
    out << std::endl << "Lower matrix:" << std::endl;
#ifdef TRANSPOSE_L
    print_matrix_L(lu.m_L);
#else
    print_matrix(lu.m_L);
#endif
    out << std::endl << "Upper matrix:" << std::endl;
    print_matrix(lu.m_U);

    return out;
}

int main(int argc, char *argv[]) {
    if (argc <= 1 || argc > 3) {
        std::cout << "LU decomposition of a square matrix." << std::endl;
        std::cout << std::endl << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << " input_matrix.bin [output.bin]" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file;
    if (argc == 3) {
        output_file = argv[2];
    }

    LU lu;
    lu.read_matrix_from_input_file(input_file);

    auto start = std::chrono::high_resolution_clock::now();
    lu.decompose();
    auto runtime = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "Computational time: " << runtime << "s" << std::endl;

    // Decomposition is printed only if the output file is not written.
    if (output_file.empty()) {
        std::cout << lu << std::endl;
    } else {
        lu.write_results_to_output_file(output_file);
    }
    return 0;
}

