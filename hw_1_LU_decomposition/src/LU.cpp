// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

//#define LINEAR

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
            //reset barrier
            counter = 0;
        }
        lk.unlock();
    }
};

class LU {
private:
    size_t m_num_threads = 1;

    std::vector<std::vector<double>> m_A;
    std::vector<std::vector<double>> m_L;
    std::vector<std::vector<double>> m_L_T;
    std::vector<std::vector<double>> m_U;

    std::vector<size_t> m_threads_indexes;

    std::condition_variable m_cv_update_A;
    std::condition_variable m_cv_decompose;

    std::mutex m_mutex_update_A;
    std::mutex m_mutex_decompose;

    std::atomic<bool> m_end = false;
    std::atomic<bool> m_update_A = false;
    std::atomic<bool> m_decompose_A = false;

    size_t m_size{};
    size_t m_k{};

    barrier m_barrier_update_A{m_num_threads};


    friend std::ostream &operator<<(std::ostream &, const LU &);

public:
    LU() {
        m_threads_indexes.resize(m_num_threads * 2, 0);
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
        m_L_T = m_L = m_U = m_A;
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

        size_t n = m_A.size();

#ifndef LINEAR
        for (size_t i = 0; i < m_A.size(); ++i) {
            for (size_t j = 0; j < m_A.size(); ++j) {
                m_L_T[i][j] = m_L[j][i];
            }
        }
#endif

        for (size_t r = 0; r < n; ++r) {
#ifdef LINEAR
            bout.write((char *) m_L[r].data(), n * sizeof(double));
#else
            bout.write((char *) m_L_T[r].data(), n * sizeof(double));
#endif
        }
        for (size_t r = 0; r < n; ++r) {
            bout.write((char *) m_U[r].data(), n * sizeof(double));
        }
    }

    [[maybe_unused]] void decompose_linear() {
        for (size_t k = 0; k < m_A.size(); ++k) {
            for (size_t j = k; j < m_A.size(); ++j) {
                m_U[k][j] = m_A[k][j];
            }
            m_L[k][k] = 1;
            for (size_t i = k + 1; i < m_A.size(); ++i) {
                m_L[i][k] = m_A[i][k] / m_U[k][k];
            }

            for (size_t i = k + 1; i < m_A.size(); ++i) {
                for (size_t j = k + 1; j < m_A.size(); ++j) {
                    auto result = m_A[i][j] - m_L[i][k] * m_U[k][j];
                    m_A[i][j] = result;
                }
            }
        }
    }

    [[maybe_unused]] void update_A(size_t begin, size_t end) {
        for (size_t i = begin; i < end; ++i) {
            for (size_t j = m_k + 1; j < m_size; ++j) {
                m_A[i][j] = m_A[i][j] - m_L[m_k][i] * m_U[m_k][j];

            }
        }
    }

    [[maybe_unused]] void decompose_parallel_1() {

        for (m_k = 0; m_k < m_size; ++m_k) {
            for (size_t j = m_k; j < m_size; ++j) {
                m_U[m_k][j] = m_A[m_k][j];
            }
            m_L[m_k][m_k] = 1;
            for (size_t i = m_k + 1; i < m_size; ++i) {
                m_L[m_k][i] = m_A[i][m_k] / m_U[m_k][m_k];
            }

            size_t sz = m_size;
            size_t middle = (m_k + 1 + m_size) / 2;
//            std::thread t1{&LU::update_A, this, m_k + 1, sz};
            std::thread t1{&LU::update_A, this, m_k + 1, middle / 4};
            std::thread t2{&LU::update_A, this, middle / 4, middle / 2};
            std::thread t3{&LU::update_A, this, middle / 2, middle / 4 * 3};
            std::thread t4{&LU::update_A, this, middle / 4 * 3, sz};
            t1.join();
            t2.join();
            t3.join();
            t4.join();

        }
    }

    void decompose() {
#ifdef LINEAR
        decompose_linear();
#else
        decompose_parallel_1();
#endif
    }

    [[maybe_unused]] void parallel_update_A(size_t idx) {
        std::unique_lock<std::mutex> l{m_mutex_update_A};

        while (true) {
            m_cv_update_A.wait(l, [this] { return m_update_A.load(std::memory_order_acquire); });
            if (idx != 0) l.unlock();
            m_update_A = false;
            std::cout << "update_a started " << idx << std::endl;
            if (m_end.load(std::memory_order_acquire)) {
                m_cv_decompose.notify_one();
                break;
            }
            if (m_num_threads < idx)
                break;

            auto begin = m_threads_indexes[idx];
            auto end = m_threads_indexes[idx + 1];

            for (size_t i = begin; i < end; ++i) {
                for (size_t j = 0; j < m_size; ++j) {
                    m_A[i][j] = m_A[i][j] - m_L[i][m_k] * m_U[m_k][j];
                }
            }

            m_barrier_update_A.wait();
            std::cout << "update_a finished " << idx << std::endl;

            if (idx == 0) {
                m_decompose_A = true;
                m_cv_decompose.notify_one();
            }

            if (m_end.load(std::memory_order_acquire)) break;
        }


    }

    [[maybe_unused]] void parallel_manager() {
        std::vector<std::thread> vector_of_threads;
        for (size_t i = 0; i < m_num_threads; ++i) {
            vector_of_threads.emplace_back(&LU::parallel_update_A, this, i);
        }
        for (auto &t: vector_of_threads) {
            t.join();
        }

    }

    [[maybe_unused]] void decompose_parallel_2() {
        std::cout << "parallel_2" << std::endl;
        std::thread t_parallel_manager{&LU::parallel_manager, this};
        size_t index_counter, counter, begin_counter, end_counter;
        std::unique_lock<std::mutex> l(m_mutex_decompose);

        for (m_k = 0; m_k < m_size; ++m_k) {
//            std::cout << m_k << std::endl;

            std::cout << "parallel started" << std::endl;

            for (size_t j = m_k; j < m_size; ++j) {
                m_U[m_k][j] = m_A[m_k][j];
            }
            m_L[m_k][m_k] = 1;
            for (size_t i = m_k + 1; i < m_size; ++i) {
                m_L[i][m_k] = m_A[i][m_k] / m_U[m_k][m_k];
            }
//            std::cout << m_k << std::endl;
            auto tmp_size = (m_size - (m_k + 1));
            m_num_threads = std::max(std::min(m_num_threads, tmp_size / 2 - 1), static_cast<size_t>(1));
            auto step = tmp_size / m_num_threads;

            index_counter = 0;
            counter = 0;
            begin_counter = m_k + 1;
            end_counter = m_k + 1 + step;

            for (; counter < m_num_threads - 1; ++counter) {
                m_threads_indexes[index_counter++] = begin_counter;

                m_threads_indexes[index_counter++] = end_counter;
                begin_counter += step + 1;
                end_counter += step + 1;
            }

            m_threads_indexes[index_counter] = begin_counter;
            ++index_counter;
            m_threads_indexes[index_counter] = m_size - 1;

            std::cout << "parallel finished" << std::endl;

            m_update_A = true;
            m_cv_update_A.notify_all();

            m_cv_decompose.wait(l, [this] { return m_decompose_A.load(std::memory_order_acquire); });
            m_decompose_A = false;
        }

        m_end = true;
        m_cv_update_A.notify_all();
        t_parallel_manager.join();
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

    std::function<void(const std::vector<std::vector<double>> &)> print_L = [&](
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

#ifdef LINEAR
    print_matrix(lu.m_L);
#else
    print_L(lu.m_L);
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
//    FOR DEBUGGING
//    std::cout << lu << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    lu.decompose();
    auto runtime = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "Computational time: " << runtime << "s" << std::endl;

    // Decomposition is printed only if the output file is not written.
//    if (output_file.empty()) {
//        std::cout << lu << std::endl;
//    } else {
//        lu.write_results_to_output_file(output_file);
//    }

    return 0;
}

