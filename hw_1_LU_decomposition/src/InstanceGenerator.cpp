#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Instance generator for LU decomposition homework assignment." << std::endl;
        std::cout << std::endl << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << " MATRIX_SIZE INSTANCE_PATH" << std::endl;
        return 1;
    }

    uint64_t n = atoi(argv[1]);
    std::string matrix_file = argv[2];

    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<double> val(1.0, 10.0);
    std::vector<std::vector<double>> A(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = val(gen);
        }
    }

    std::ofstream bout(matrix_file.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    if (bout.fail()) {
        std::cerr << "Cannot write to '" << matrix_file << "' file!" << std::endl;
        return 1;
    }

    bout.write((char *) &n, sizeof(uint64_t));
    for (uint64_t r = 0; r < n; ++r) {
        bout.write((char *) A[r].data(), n * sizeof(double));
    }

    return 0;
}

