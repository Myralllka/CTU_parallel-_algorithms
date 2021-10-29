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
    int m_x;
    int m_y;
    float m_temperature;

    bool operator==(const Spot &b) const {
        return (m_x == b.m_x) && (m_y == b.m_y);
    }
};

std::tuple<int, int, std::vector<Spot>> read_instance(std::string instance_file_name) {
    int width, height;
    std::vector<Spot> spots;
    std::string line;

    std::ifstream file(instance_file_name);
    if (file.is_open()) {
        int line_id = 0;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            if (line_id == 0) {
                ss >> width;
            } else if (line_id == 1) {
                ss >> height;
            } else {
                int i, j, temperature;
                ss >> i >> j >> temperature;
                spots.push_back({i, j, (float) temperature});
            }
            line_id++;
        }
        file.close();
    } else {
        throw std::runtime_error("It is not possible to open instance file!\n");
    }
    return make_tuple(width, height, spots);
}

void write_output(
        const int my_rank,
        const int width,
        const int height,
        const std::string output_file_name,
        const std::vector<float> &temperatures) {
    // Draw the output image in Netpbm format.
    std::ofstream file(output_file_name);
    if (file.is_open()) {
        if (my_rank == 0) {
            file << "P2\n" << width << "\n" << height << "\n" << 255 << "\n";
            for (auto temperature: temperatures) {
                file << (int) std::max(std::min(temperature, 255.0f), 0.0f) << " ";
            }
        }
    }
}

void print_help_page(char *program) {
    std::cout << "Simulates a simple heat diffusion." << std::endl;
    std::cout << std::endl << "Usage:" << std::endl;
    std::cout << "\t" << program << " INPUT_PATH OUTPUT_PATH" << std::endl << std::endl;
}

int main(int argc, char **argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    int world_size, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (argc == 1) {
        if (my_rank == 0) {
            print_help_page(argv[0]);
        }
        MPI_Finalize();
        exit(0);
    } else if (argc != 3) {
        if (my_rank == 0) {
            print_help_page(argv[0]);
        }
        MPI_Finalize();
        exit(1);
    }

    // Read the input instance.
    int width, height;  // Width and height of the matrix.
    std::vector<Spot> spots; // Spots with permanent temperature.
    if (my_rank == 0) {
        tie(width, height, spots) = read_instance(argv[1]);
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

    double total_duration = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - start).count();
    std::cout << "computational time: " << total_duration << " s" << std::endl;

    if (my_rank == 0) {
        std::string output_file_name(argv[2]);
        write_output(my_rank, width, height, output_file_name, temperatures);
    }

    MPI_Finalize();
    return 0;
}

