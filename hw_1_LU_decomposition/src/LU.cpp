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

class LU {
private:
    std::vector<std::vector<double>> m_A;
    std::vector<std::vector<double>> m_L;
    std::vector<std::vector<double>> m_U;

    friend std::ostream &operator<<(std::ostream &, const LU &);

public:
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

    void decompose() {
        decompose_linear();
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

    [[maybe_unused]] void decompose_parallel() {

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
    std::cout << lu << std::endl;
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

