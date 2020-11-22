// Copyright 2020 Vorobev Leonid
#include <mpi.h>
#include <time.h>
#include <vector>
#include <random>
#include "../../modules/task_1/vorobev_l_mid/mid.h"

std::vector<int> randvector(int size) {
    std::mt19937 gen;
    gen.seed(time(0));

    std::vector<int> vec(size);
    for (int i = 0; i < size; i++) {
        vec[i] = gen();
    }
    return vec;
}

int avgvector(std::vector<int> vec, int sizevec) {
    int sum = 0;
    for (int i = 0; i < sizevec; i++) {
        sum += vec[i];
    }
    return static_cast<int>(sum) / sizevec;
}

int avgvectorpar(std::vector<int> vec, int sizevec) {
    int sum, allsum, num, rank, size;

    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Bcast(&sizevec, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != num - 1) {
        size = sizevec / num;
    } else {
        size = sizevec / num + sizevec % num;
    }
    if (rank == 0) {
        for (int i = 1; i < num - 1; i++) {
            MPI_Send(&vec[0] + sizevec / num * i, size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        if (num > 1) {
            MPI_Send(&vec[0] + sizevec / num * (num - 1),
                size + sizevec % num, MPI_INT, (num - 1), 0, MPI_COMM_WORLD);
        }
    }

    std::vector<int> localvector(size);

    if (rank == 0) {
        localvector = std::vector<int>(vec.begin(), vec.begin() + size);
    } else {
        MPI_Status status;
        MPI_Recv(&localvector[0], size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    sum = 0;
    for (int i = 0; i < size; i++) {
        sum += localvector[i];
    }
    MPI_Reduce(&sum, &allsum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    return static_cast<int>(allsum) / sizevec;
}
