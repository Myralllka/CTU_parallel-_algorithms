// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "LU.h"

void LU::read_matrix_from_input_file(const std::string &input_file) {
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

void LU::write_results_to_output_file(const std::string &output_file) {
    std::ofstream bout(output_file.c_str(), std::ofstream::out | std::ofstream::binary);
    if (bout.fail()) {
        throw std::invalid_argument("Cannot open the output file!");
    }
    for (size_t i = 0; i < m_size; ++i) {
        for (size_t j = i; j < m_size; ++j) {
            std::swap(m_L[j][i], m_L[i][j]);
        }
    }
    for (size_t r = 0; r < m_size; ++r) {
        bout.write((char *) m_L[r].data(), m_size * sizeof(double));
    }
    for (size_t r = 0; r < m_size; ++r) {
        bout.write((char *) m_U[r].data(), m_size * sizeof(double));
    }

}

[[maybe_unused]] void LU::decompose_linear() {
    for (m_k = 0; m_k < m_size; ++m_k) {
        m_U[m_k][m_k] = m_A[m_k][m_k];
        m_L[m_k][m_k] = 1;
        for (size_t j = m_k + 1; j < m_size; ++j) {
            m_U[m_k][j] = m_A[m_k][j];
            m_L[m_k][j] = m_A[j][m_k] / m_U[m_k][m_k];
        }

        for (size_t i = m_k + 1; i < m_size; ++i) {
            for (size_t j = m_k + 1; j < m_size; ++j) {
                m_A[i][j] = m_A[i][j] - m_L[m_k][i] * m_U[m_k][j];
            }
        }
    }
}

void LU::decompose() {
    if (m_num_threads >= m_A.size()) {
        decompose_linear();
    } else {
        decompose_parallel();
    }
}

[[maybe_unused]] void LU::parallel_update_A(size_t idx) {
    size_t k, s, step, begin, end;
    std::vector<double> v;
    v.resize(m_size);
    s = m_size;
    for (size_t c = 0; c < s; ++c) {
        m_barrier_update_ready_A.wait();
        k = m_k;

        step = (s - (k + 1)) / (m_num_threads);

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

[[maybe_unused]] void LU::decompose_parallel() {
    std::vector<std::thread> threads_vec;

    double el;
    size_t tmp_size, step, begin;

    threads_vec.reserve(m_num_threads - 1);

    for (size_t i = 0; i < m_num_threads - 1; ++i) {
        threads_vec.emplace_back(&LU::parallel_update_A, this, i);
    }

    for (m_k = 0; m_k < m_size; ++m_k) {
        m_U[m_k][m_k] = m_A[m_k][m_k];
        m_L[m_k][m_k] = 1;
        for (size_t j = m_k + 1; j < m_size; ++j) {
            m_U[m_k][j] = m_A[m_k][j];
            m_L[m_k][j] = m_A[j][m_k] / m_U[m_k][m_k];
        }

        m_barrier_update_ready_A.wait();

        tmp_size = (m_size - (m_k + 1));
        step = tmp_size / (m_num_threads);

        begin = m_k + 1 + (m_num_threads - 1) * step;
        for (size_t i = begin; i < m_size; ++i) {
            el = m_L[m_k][i];
            for (size_t j = 0; j < m_size; ++j) {
                m_A[i][j] -= el * m_U[m_k][j];
            }
        }
        m_barrier_update_A.wait();

    }
    for (auto &t: threads_vec) {
        t.join();
    }
}


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

//    out << "Matrix A:" << std::endl;
//    print_matrix(lu.m_A);
    out << std::endl << "Lower matrix:" << std::endl;
    print_matrix_L(lu.m_L);
    out << std::endl << "Upper matrix:" << std::endl;
    print_matrix(lu.m_U);

    return out;
}

