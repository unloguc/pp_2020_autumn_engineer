// Copyright 2020 Kustova Anastasiya
#include <mpi.h>
#include <time.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>


std::vector<int> getRandomVector() {
    unsigned int k = time(NULL) % 100;
    int n = rand_r(&k) % 100;
    std::vector<int> vec(n);
    for (int  i = 0; i < n; i++) { vec[i] = rand_r(&k) % 100; }
    return vec;
}

int getSequentialOperations(std::vector<int> vec) {
    const int  sz = vec.size();
    int reduction_elem = vec[0];
    for (int  i = 1; i < sz; i++) {
        reduction_elem = std::max(reduction_elem, vec[i]);
    }
    return reduction_elem;
}

int getParallelOperations(std::vector<int> global_vec) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int delta = global_vec.size() / size;

    if (rank == 0) {
        for (int proc = 1; proc < size; proc++) {
            MPI_Send(&global_vec[0] + proc * delta, delta,
                        MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    }

    std::vector<int> local_vec(delta);
    if (rank == 0) {
        local_vec = std::vector<int>(global_vec.begin(),
                                     global_vec.begin() + delta);
    } else {
        MPI_Status status;
        MPI_Recv(&local_vec[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int global_sum = 0;
    int local_sum = getSequentialOperations(local_vec);
    MPI_Op op_code;
    op_code = MPI_MAX;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return global_sum;
}
