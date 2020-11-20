// Copyright 2020 Kustova Anastasiya
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>

std::vector<int> generateVector(int n) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(n);
    for (int i = 0; i < n; i++) { vec[i] = gen() % 100; }
    return vec;
}

int getLocalMax(std::vector<int> vec) {
    const int  len = vec.size();
    int current_elem = vec[0];
    for (int  i = 1; i < len; i++) {
        current_elem = std::max(current_elem, vec[i]);
    }
    return current_elem;
}

int getParallelMax(std::vector<int> vec, int len_vector) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int delta = len_vector / size;
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Send(&vec[0] + i * delta, delta,
                        MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    std::vector<int> local_vec(delta);
    if (rank == 0) {
        local_vec = std::vector<int>(vec.begin(), vec.begin() + delta);
    } else {
        MPI_Status status;
        MPI_Recv(&local_vec[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int total_sum = 0;
    int local_sum = getLocalMax(local_vec);
    MPI_Op op_code;
    op_code = MPI_MAX;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return total_sum;
}
