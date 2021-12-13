#include <iostream>
#include <vector>
#include <mpi.h>
#include <cmath>

#include "Utils.hpp"

#define ROOT_PROCESS 0

double compute_sum_of_squares(const std::vector<double> &u) {
    double sum_squares = 0;
    for (double x: u) {
        sum_squares += x * x;
    }
    return sum_squares;
}

double compute_vector_length(const std::vector<double> u) {
    return sqrt(compute_sum_of_squares(u));
}

void divide_by_vector_length(std::vector<double> &u, double vector_length) {
    for (double &i: u) {
        i /= vector_length;
    }
}

std::vector<double> distribute_input_vector_chunks(
        const std::vector<double> &u,
        const std::vector<int> &chunk_sizes,
        const std::vector<int> &bases) {
    int child_chunk_size = chunk_sizes.back();
    int root_chunk_size = chunk_sizes[ROOT_PROCESS];

    // First, we inform the slaves about the chunk size. According to that, slaves will allocate buffers to receive
    // their chunk of the vector.
    MPI_Bcast(&child_chunk_size, 1, MPI_INT, ROOT_PROCESS, MPI_COMM_WORLD);

    // Second, distribute the vector chunks (the first chunk of size root_chunk_size is for the root process).
    std::vector<double> buf(root_chunk_size, 0);      // Root process will also receive its chunk.
    MPI_Scatterv(u.data(),
                 chunk_sizes.data(),
                 bases.data(),
                 MPI_DOUBLE,
                 buf.data(),
                 root_chunk_size,
                 MPI_DOUBLE,
                 ROOT_PROCESS,
                 MPI_COMM_WORLD);

    return buf;
}

std::vector<double> receive_input_vector_chunk() {
    // Get the chunk size.
    int chunk_size;
    MPI_Bcast(&chunk_size, 1, MPI_INT, ROOT_PROCESS, MPI_COMM_WORLD);

    // Get the chunk of the input vector.
    std::vector<double> buf(chunk_size, 0);
    MPI_Scatterv(nullptr,
                 nullptr,
                 nullptr,
                 MPI_DOUBLE,
                 buf.data(),
                 chunk_size,
                 MPI_DOUBLE,
                 ROOT_PROCESS,
                 MPI_COMM_WORLD);

    return buf;
}

double receive_vector_length(double sum_of_squares_chunk) {
    double total_sum_of_squares = 0.0;
    MPI_Allreduce(&sum_of_squares_chunk,
                  &total_sum_of_squares,
                  1,
                  MPI_DOUBLE,
                  MPI_SUM,
                  MPI_COMM_WORLD);

    return sqrt(total_sum_of_squares);
}

void send_processed_vector_chunk(const std::vector<double> &u_chunk) {
    MPI_Gatherv(u_chunk.data(),
                u_chunk.size(),
                MPI_DOUBLE,
                nullptr,
                nullptr,
                nullptr,
                MPI_DOUBLE,
                ROOT_PROCESS,
                MPI_COMM_WORLD);
}

std::vector<double> receive_output_vector(
        const std::vector<double> &u_chunk,
        int output_vector_size,
        const std::vector<int> &chunk_sizes,
        const std::vector<int> &bases) {
    std::vector<double> buf(output_vector_size, 0);
    MPI_Gatherv(u_chunk.data(),
                u_chunk.size(),
                MPI_DOUBLE,
                buf.data(),
                chunk_sizes.data(),
                bases.data(),
                MPI_DOUBLE,
                ROOT_PROCESS,
                MPI_COMM_WORLD);

    return buf;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (my_rank == ROOT_PROCESS) {
        std::vector<double> u = generate_random_vector(5000000);

        Stopwatch sw;
        sw.start();

        int child_chunk_size = u.size() / world_size;
        int root_chunk_size = u.size() - (child_chunk_size * (world_size - 1));

        std::vector<int> chunk_sizes(world_size, 0);
        std::vector<int> bases(world_size, 0);
        for (int i = 0; i < world_size; i++) {
            if (i == ROOT_PROCESS) {
                chunk_sizes[i] = root_chunk_size;
                bases[i] = 0;
            } else {
                chunk_sizes[i] = child_chunk_size;
                bases[i] = root_chunk_size + (i - 1) * child_chunk_size;
            }
        }

        std::vector<double> u_chunk = distribute_input_vector_chunks(u, chunk_sizes, bases);
        double sum_of_squares_chunk = compute_sum_of_squares(u_chunk);
        double vector_length = receive_vector_length(sum_of_squares_chunk);
        divide_by_vector_length(u_chunk, vector_length);
        auto u_norm = receive_output_vector(u_chunk, u.size(), chunk_sizes, bases);

        sw.stop();
        std::cout << "MPI: " << sw.duration().count() << " ms, length " << compute_vector_length(u_norm) << std::endl;
    } else {
        std::vector<double> u_chunk = receive_input_vector_chunk();
        double sum_of_squares_chunk = compute_sum_of_squares(u_chunk);
        double vector_length = receive_vector_length(sum_of_squares_chunk);
        divide_by_vector_length(u_chunk, vector_length);
        send_processed_vector_chunk(u_chunk);
    }

    MPI_Finalize();

    return 0;
}
