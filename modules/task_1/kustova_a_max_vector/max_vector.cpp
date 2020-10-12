// Copyright 2020 Kustova Anastasiya
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>

/*
std::vector<int> getRandomVector(int n) {
    std::vector<int> vec(n);
    unsigned int k = time(NULL) % 100;
    for (int  i = 0; i < n; i++) { vec[i] = rand_r(&k) % 100; }
    return vec;
}*/

std::vector<int> getRandomVector(int sz) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(sz);
    for (int  i = 0; i < sz; i++) { vec[i] = gen() % 100; }
    return vec;
}

int getSequentialOperations(std::vector<int> vec) {
    const int  sz = vec.size();
    int reduction_elem = 0;
    reduction_elem = vec[0];
    for (int  i = 1; i < sz; i++) {
        reduction_elem = std::max(reduction_elem, vec[i]);
    }
    return reduction_elem;
}

int getParallelOperations(std::vector<int> total_vec) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int vector_size = total_vec.size();
    const int delta = vector_size / size;

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Send(&total_vec[0] + i * delta, delta,
                        MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    std::vector<int> local_vec(delta);
    if (rank == 0) {
        local_vec = std::vector<int>(total_vec.begin(),
                                     total_vec.begin() + delta);
    } else {
        MPI_Status status;
        MPI_Recv(&local_vec[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int total_sum = 0;
    int local_sum = getSequentialOperations(local_vec);
    MPI_Op op_code;
    op_code = MPI_MAX;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return total_sum;
}
