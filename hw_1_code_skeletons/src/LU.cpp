#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <functional>
#include <vector>

class LU {
public:
    void readMatrixFromInputFile(const std::string &inputFile) {
        std::ifstream bin(inputFile.c_str(), std::ifstream::in | std::ifstream::binary);
        if (bin.fail()) {
            throw std::invalid_argument("Cannot open the input file!");
        }

        uint64_t n = 0;
        bin.read((char *) &n, sizeof(uint64_t));
        A.resize(n, std::vector<double>(n, 0.0));
        L = U = A;
        for (uint64_t r = 0; r < n; ++r) {
            bin.read((char *) A[r].data(), n * sizeof(double));
        }
    }

    void writeResultsToOutputFile(const std::string &outputFile) {
        std::ofstream bout(outputFile.c_str(), std::ofstream::out | std::ofstream::binary);
        if (bout.fail()) {
            throw std::invalid_argument("Cannot open the output file!");
        }

        uint64_t n = A.size();
        for (uint64_t r = 0; r < n; ++r) {
            bout.write((char *) L[r].data(), n * sizeof(double));
        }
        for (uint64_t r = 0; r < n; ++r) {
            bout.write((char *) U[r].data(), n * sizeof(double));
        }
    }

    void decompose() {
        // TODO: Implement a parallel LU decomposition (i.e., fill L and U members of this class instance).
    }

private:

    std::vector<std::vector<double>> A;
    std::vector<std::vector<double>> L;
    std::vector<std::vector<double>> U;

    friend std::ostream &operator<<(std::ostream &, const LU &);
};

// Print the matrices A, L, and U of an LU class instance.
std::ostream &operator<<(std::ostream &out, const LU &lu) {
    std::function<void(const std::vector<std::vector<double>> &)> printMatrix = [&](
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
    printMatrix(lu.A);
    out << std::endl << "Lower matrix:" << std::endl;
    printMatrix(lu.L);
    out << std::endl << "Upper matrix:" << std::endl;
    printMatrix(lu.U);

    return out;
}

int main(int argc, char *argv[]) {
    if (argc <= 1 || argc > 3) {
        std::cout << "LU decomposition of a square matrix." << std::endl;
        std::cout << std::endl << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << " inputMatrix.bin [output.bin]" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile;
    if (argc == 3) {
        outputFile = argv[2];
    }

    LU lu;
    lu.readMatrixFromInputFile(inputFile);

    auto start = std::chrono::high_resolution_clock::now();
    lu.decompose();
    auto runtime = duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "Computational time: " << runtime << "s" << std::endl;

    // Decomposition is printed only if the output file is not written.
    if (outputFile.empty()) {
        std::cout << lu << std::endl;
    } else {
        lu.writeResultsToOutputFile(outputFile);
    }

    return 0;
}

