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


class barrier {
private:
    std::mutex m_m;
    std::condition_variable m_cv;
    int counter;
    int waiting;
    int thread_count;

public:
    explicit barrier(int count) : thread_count(count), counter(0), waiting(0) {}

    void wait() {
        //fence mechanism
        std::unique_lock<std::mutex> lk(m_m);
        ++counter;
        ++waiting;
        m_cv.wait(lk, [&] { return counter >= thread_count; });
        m_cv.notify_one();
        --waiting;
        if (waiting == 0) {
            counter = 0;
        }
        lk.unlock();
    }
};

class LU {
private:
    int m_num_threads = std::thread::hardware_concurrency();
//    int m_num_threads = 4;

    std::vector<std::vector<double>> m_A;
    std::vector<std::vector<double>> m_L;
    std::vector<std::vector<double>> m_U;

    int m_size{};
    int m_k{};

    barrier m_barrier_update_A{m_num_threads};
    barrier m_barrier_update_ready_A{m_num_threads};

    friend std::ostream &operator<<(std::ostream &, const LU &);

public:
    LU() = default;

    ~LU() = default;

    void read_matrix_from_input_file(const std::string &input_file) {
        std::ifstream bin(input_file.c_str(), std::ifstream::in | std::ifstream::binary);
        if (bin.fail()) {
            throw std::invalid_argument("Cannot open the input file!");
        }

        int n = 0;
        bin.read((char *) &n, sizeof(size_t));
        m_A.resize(n, std::vector<double>(n, 0.0));
        m_L = m_U = m_A;
        for (int r = 0; r < n; ++r) {
            bin.read((char *) m_A[r].data(), n * sizeof(double));
        }
        m_size = m_A.size();
    }

    void write_results_to_output_file(const std::string &output_file) {
        std::ofstream bout(output_file.c_str(), std::ofstream::out | std::ofstream::binary);
        if (bout.fail()) {
            throw std::invalid_argument("Cannot open the output file!");
        }
        for (int i = 0; i < m_size; ++i) {
            for (int j = i; j < m_size; ++j) {
                std::swap(m_L[j][i], m_L[i][j]);
            }
        }
        for (int r = 0; r < m_size; ++r) {
            bout.write((char *) m_L[r].data(), m_size * sizeof(double));
        }
        for (int r = 0; r < m_size; ++r) {
            bout.write((char *) m_U[r].data(), m_size * sizeof(double));
        }

    }

    [[maybe_unused]] void decompose_linear() {
        for (m_k = 0; m_k < m_size; ++m_k) {
            m_U[m_k][m_k] = m_A[m_k][m_k];
            m_L[m_k][m_k] = 1;
            for (int j = m_k + 1; j < m_size; ++j) {
                m_U[m_k][j] = m_A[m_k][j];
                m_L[m_k][j] = m_A[j][m_k] / m_U[m_k][m_k];
            }

            for (int i = m_k + 1; i < m_size; ++i) {
                for (int j = m_k + 1; j < m_size; ++j) {
                    m_A[i][j] = m_A[i][j] - m_L[m_k][i] * m_U[m_k][j];
                }
            }
        }
    }

    void decompose() {
//        if (m_num_threads >= m_A.size()) {
//            decompose_linear();
//        } else {
        decompose_parallel();
//        }
    }

    [[maybe_unused]] void parallel_update_A(int idx) {
        int k, s, tmp_size, step, begin, end;
        std::vector<double> v;
        v.resize(m_size);
        s = m_size;
        for (int c = 0; c < s; ++c) {
            m_barrier_update_ready_A.wait();
            k = m_k;
            tmp_size = (s - (k + 1));
            step = tmp_size / (m_num_threads);
            begin = k + 1 + idx * step;
            end = k + 1 + (idx + 1) * step;

            for (size_t i = begin; i < end; ++i) {
                for (size_t j = 0; j < m_size; ++j) {
                    m_A[i][j] -= m_L[m_k][i] * m_U[m_k][j];
                }
            }
            m_barrier_update_A.wait();

        }
    }

    [[maybe_unused]] void decompose_parallel() {
        std::vector<std::thread> threads_vec;

        double el;
        int tmp_size, step, begin;

        threads_vec.reserve(m_num_threads - 1);

        for (int i = 0; i < m_num_threads - 1; ++i) {
            threads_vec.emplace_back(&LU::parallel_update_A, this, i);
        }

        for (m_k = 0; m_k < m_size; ++m_k) {
            m_U[m_k][m_k] = m_A[m_k][m_k];
            m_L[m_k][m_k] = 1;
            for (int j = m_k + 1; j < m_size; ++j) {
                m_U[m_k][j] = m_A[m_k][j];
                m_L[m_k][j] = m_A[j][m_k] / m_U[m_k][m_k];
            }

            m_barrier_update_ready_A.wait();
//            for (int i = m_k + 1 + (m_num_threads - 1); i < m_size; i += m_num_threads) {


            tmp_size = (m_size - (m_k + 1));
            step = tmp_size / (m_num_threads);

            begin = m_k + 1 + (m_num_threads - 1) * step;
            for (int i = begin; i < m_size; ++i) {
                el = m_L[m_k][i];
                for (int j = 0; j < m_size; ++j) {
                    m_A[i][j] -= el * m_U[m_k][j];
                }
            }
            m_barrier_update_A.wait();

        }
        for (auto &t: threads_vec) {
            t.join();
        }
    }
};

// Print the matrices A, L, and U of an LU class instance.
std::ostream &operator<<(std::ostream &out, const LU &lu) {

    std::function<void(const std::vector<std::vector<double>> &)> print_matrix = [&](
            const std::vector<std::vector<double>> &M) {

        int n = M.size();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                out << " " << std::setw(10) << M[i][j];
            }
            out << std::endl;
        }
    };

    std::function<void(const std::vector<std::vector<double>> &)> print_matrix_L = [&](
            const std::vector<std::vector<double>> &M) {

        int n = M.size();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                out << " " << std::setw(10) << M[j][i];
            }
            out << std::endl;
        }
    };

//    out << "Matrix A:" << std::endl;
//    print_matrix(lu.m_A);
    out << std::endl << "Lower matrix:" << std::endl;
    print_matrix_L(lu.m_L);
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

