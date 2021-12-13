#include <iostream>
#include <mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    // What is my rank (process ID)?
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // How many processes do we have in total?
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout << "My rank: " << my_rank << std::endl;

    if (my_rank == 0) {
        std::cout << "Total number of processes: " << size << std::endl;
    }


    MPI_Finalize();
}
