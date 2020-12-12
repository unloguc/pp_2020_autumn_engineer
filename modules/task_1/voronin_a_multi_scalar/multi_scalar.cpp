// Copyright 2020 Voronin Aleksey
#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../../modules/task_1/voronin_a_multi_scalar/multi_scalar.h"


std::vector<int> getRandomVector(int size) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(size);
    for (int  i = 0; i < size; i++) { vec[i] = gen() % 100; }
    return vec;
}

int getSequentialOperations(std::vector<int> vec, std::vector<int> second_vec) {
    const int  sz = vec.size();
    int sum = 0;

        for (int  i = 0; i < sz; i++) {
            sum += vec[i]*second_vec[i];
        }
    return sum;
}

int getParallelOperations(std::vector<int> global_vec, std::vector<int> second_global_vec,
                          int count_size_vector) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int delta = count_size_vector / size;

    if (rank == 0) {
        for (int proc = 1; proc < size; proc++) {
            MPI_Send(&global_vec[0] + proc * delta, delta,
                        MPI_INT, proc, 0, MPI_COMM_WORLD);
            MPI_Send(&second_global_vec[0] + proc * delta, delta,
                        MPI_INT, proc, 1, MPI_COMM_WORLD);
        }
    }

    std::vector<int> local_vec(delta);
    std::vector<int> second_local_vec(delta);
    if (rank == 0) {
        local_vec = std::vector<int>(global_vec.begin(),
                                     global_vec.begin() + delta);
        second_local_vec = std::vector<int>(second_global_vec.begin(),
                                     second_global_vec.begin() + delta);
    } else {
        MPI_Status status;
        MPI_Recv(&local_vec[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&second_local_vec[0], delta, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    }

    int global_sum = 0;
    int local_sum = getSequentialOperations(local_vec, second_local_vec);
    MPI_Op op_code;
     op_code = MPI_SUM;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return global_sum;
}
