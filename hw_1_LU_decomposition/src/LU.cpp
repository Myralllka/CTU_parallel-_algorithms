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
//    const size_t m_num_threads = std::thread::hardware_concurrency();
    const size_t m_num_threads = 2;

    std::vector<std::vector<double>> m_A;
    std::vector<std::vector<double>> m_L;
    std::vector<std::vector<double>> m_U;

    std::vector<size_t> m_threads_indexes;

    std::condition_variable m_cv_parallel_manager;
    std::condition_variable m_cv_update_A;
    std::condition_variable m_cv_decompose;

    std::mutex m_mutex_parallel_manager;
    std::mutex m_mutex_update_A;
    std::mutex m_mutex_decompose;

    bool m_end = false;
    bool m_start_manager = false;
    bool m_decompose_continue = false;
//    bool ready = false;

    size_t m_size{};
    size_t m_k{};

    barrier m_barrier_update_A{m_num_threads};


    friend std::ostream &operator<<(std::ostream &, const LU &);

public:
    LU() {
        m_threads_indexes.resize(m_num_threads, 0);
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
        m_L = m_U = m_A;
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
        for (size_t r = 0; r < n; ++r) {
            bout.write((char *) m_L[r].data(), n * sizeof(double));
        }
        for (size_t r = 0; r < n; ++r) {
            bout.write((char *) m_U[r].data(), n * sizeof(double));
        }
    }

    [[maybe_unused]] void decompose_linear() {
        std::cout << "linear" << std::endl;
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
                m_A[i][j] = m_A[i][j] - m_L[i][m_k] * m_U[m_k][j];

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
                m_L[i][m_k] = m_A[i][m_k] / m_U[m_k][m_k];
            }

            size_t sz = m_size;
            size_t middle = (m_k + 1 + m_size) / 2;
//            std::thread t1{&LU::update_A, this, m_k + 1, sz};
            std::thread t1{&LU::update_A, this, m_k + 1, middle / 2};
            std::thread t2{&LU::update_A, this, middle / 2, sz};
            t1.join();
            t2.join();

        }
    }

    void decompose() {
        if (m_A.size() < m_num_threads)
            decompose_linear();
        else
            decompose_parallel_2();
    }

    [[maybe_unused]] void parallel_update_A() {
        std::unique_lock<std::mutex> l(m_mutex_update_A, std::defer_lock);
        while (!m_end) {

        }
    }

    [[maybe_unused]] void parallel_manager() {
        std::unique_lock<std::mutex> l(m_mutex_parallel_manager);
        std::vector<std::thread> vector_of_threads;
        for () {

        }

    }

    [[maybe_unused]] void decompose_parallel_2() {
        std::cout << "parallel_2" << std::endl;
        std::thread t_parallel_manager{&LU::parallel_manager, this};
        std::unique_lock<std::mutex> l(m_mutex_decompose, std::defer_lock);

        for (m_k = 0; m_k < m_size; ++m_k) {

            for (size_t j = m_k; j < m_size; ++j) {
                m_U[m_k][j] = m_A[m_k][j];
            }
            m_L[m_k][m_k] = 1;
            for (size_t i = m_k + 1; i < m_size; ++i) {
                m_L[i][m_k] = m_A[i][m_k] / m_U[m_k][m_k];
            }


        }

        t_parallel_manager.join();
    }
//    [[maybe_unused]] void parallel_update_A(size_t begin, size_t end) {
//        auto size = m_A.size();
//
//        std::unique_lock<std::mutex> l(m_mutex_update_A, std::defer_lock);
//
//        while (true) {
//            l.lock();
//            m_cv_update_A.wait(l, []{return true;});
//            l.unlock();
//            if (m_end) break;
//
//            for (size_t i = begin; i < end; ++i) {
//                for (size_t j = 0; j < size; ++j) {
//                    m_A[i][j] = m_A[i][j] - m_L[i][m_k] * m_U[m_k][j];
//                }
//            }
//        }
//
//        m_barrier_update_A.wait();
//
//        m_cv_decompose.notify_all();
//
//        std::cout << "parallel_update_A ends" << std::endl;
//
//    }

//    [[maybe_unused]] void parallel_manager() {
//        std::unique_lock<std::mutex> l(m_mutex_parallel, std::defer_lock);
//        size_t size = m_size;
//
//        std::thread t1{&LU::parallel_update_A, this, 0, size};
////        std::thread t2{&LU::parallel_update_A, this, size / 2, size, k};
//
//
//        while (true) {
//            l.lock();
//
//            m_cv_parallel.wait(l, [] { return true; });
//
//            m_cv_update_A.notify_all();
//
//            l.unlock();
//            if (m_end) break;
//        }
//
//        t1.join();
//        std::cout << "parallel_manager ends" << std::endl;
////        t2.join();
//    }

//    [[maybe_unused]] void decompose_parallel() {
//        m_size = m_A.size();
//        std::unique_lock<std::mutex> l(m_mutex_decompose, std::defer_lock);
//
//        std::thread manager(&LU::parallel_manager, this);
//
//        for (m_k = 0; m_k < m_size; ++m_k) {
//
//            for (size_t j = m_k; j < m_size; ++j) {
//                m_U[m_k][j] = m_A[m_k][j];
//            }
//
//            m_L[m_k][m_k] = 1;
//            for (size_t i = m_k + 1; i < m_size; ++i) {
//                m_L[i][m_k] = m_A[i][m_k] / m_U[m_k][m_k];
//            }
//
//            m_cv_parallel.notify_all();
//            l.lock();
//            m_cv_decompose.wait(l, []{return true;});
//            l.unlock();
//        }
//        m_end = true;
//        manager.join();
//    }
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

    out << "Matrix A:" << std::endl;
    print_matrix(lu.m_A);
    out << std::endl << "Lower matrix:" << std::endl;
    print_matrix(lu.m_L);
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

