//
// Created by myralllka on 10/26/21.
//
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <functional>
#include <iomanip>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cout << "Instance generator for LU decomposition homework assignment." << std::endl;
        std::cout << std::endl << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << "SIZE INSTANCE_PATH" << std::endl;
        return 1;
    }
    std::ifstream bin(argv[2], std::ifstream::in | std::ifstream::binary);

    if (bin.fail()) {
        throw std::invalid_argument("Cannot open the input file!");
    }

    size_t n = atoi(argv[1]);
//    bin.read((char *) &n, sizeof(size_t));
    std::vector<std::vector<double>> U, L;
    U.resize(n, std::vector<double>(n, 0.0));
    L.resize(n, std::vector<double>(n, 0.0));
    for (size_t r = 0; r < n; ++r) {
        bin.read((char *) L[r].data(), n * sizeof(double));
    }
    for (size_t r = 0; r < n; ++r) {
        bin.read((char *) U[r].data(), n * sizeof(double));
    }

    std::function<void(const std::vector<std::vector<double>> &)> print_matrix = [&](
            const std::vector<std::vector<double>> &M) {

        size_t n = M.size();
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                std::cout << " " << std::setw(10) << M[i][j];
            }
            std::cout << std::endl;
        }
    };
    std::cout << std::endl << "Upper matrix:" << std::endl;
    print_matrix(U);
    std::cout << std::endl << "Lower matrix:" << std::endl;
    print_matrix(L);


}