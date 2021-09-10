#include <mpi.h>
#include <chrono>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <tuple>
#include <iostream>

// Spot with permanent temperature on coordinates [x,y].
struct Spot {
    int mX;
    int mY;
    float mTemperature;

    bool operator==(const Spot &b) const {
        return (mX == b.mX) && (mY == b.mY);
    }
};

std::tuple<int, int, std::vector<Spot>> readInstance(std::string instanceFileName) {
    int width, height;
    std::vector<Spot> spots;
    std::string line;

    std::ifstream file(instanceFileName);
    if (file.is_open()) {
        int lineId = 0;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            if (lineId == 0) {
                ss >> width;
            } else if (lineId == 1) {
                ss >> height;
            } else {
                int i, j, temperature;
                ss >> i >> j >> temperature;
                spots.push_back({i, j, (float) temperature});
            }
            lineId++;
        }
        file.close();
    } else {
        throw std::runtime_error("It is not possible to open instance file!\n");
    }
    return make_tuple(width, height, spots);
}

void writeOutput(
        const int myRank,
        const int width,
        const int height,
        const std::string outputFileName,
        const std::vector<float> &temperatures) {
    // Draw the output image in Netpbm format.
    std::ofstream file(outputFileName);
    if (file.is_open()) {
        if (myRank == 0) {
            file << "P2\n" << width << "\n" << height << "\n" << 255 << "\n";
            for (auto temperature: temperatures) {
                file << (int) std::max(std::min(temperature, 255.0f), 0.0f) << " ";
            }
        }
    }
}

void printHelpPage(char *program) {
    std::cout << "Simulates a simple heat diffusion." << std::endl;
    std::cout << std::endl << "Usage:" << std::endl;
    std::cout << "\t" << program << " INPUT_PATH OUTPUT_PATH" << std::endl << std::endl;
}

int main(int argc, char **argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    int worldSize, myRank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if (argc == 1) {
        if (myRank == 0) {
            printHelpPage(argv[0]);
        }
        MPI_Finalize();
        exit(0);
    } else if (argc != 3) {
        if (myRank == 0) {
            printHelpPage(argv[0]);
        }
        MPI_Finalize();
        exit(1);
    }

    // Read the input instance.
    int width, height;  // Width and height of the matrix.
    std::vector<Spot> spots; // Spots with permanent temperature.
    if (myRank == 0) {
        tie(width, height, spots) = readInstance(argv[1]);
    }

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();


    //-----------------------\\
    // Insert your code here \\
    //        |  |  |        \\
    //        V  V  V        \\

    // TODO: Fill this array on processor with rank 0. It must have height * width elements and it contains the
    // linearized matrix of temperatures in row-major order
    // (see https://en.wikipedia.org/wiki/Row-_and_column-major_order)
    std::vector<float> temperatures;

    //-----------------------\\

    double totalDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - start).count();
    std::cout << "computational time: " << totalDuration << " s" << std::endl;

    if (myRank == 0) {
        std::string outputFileName(argv[2]);
        writeOutput(myRank, width, height, outputFileName, temperatures);
    }

    MPI_Finalize();
    return 0;
}

