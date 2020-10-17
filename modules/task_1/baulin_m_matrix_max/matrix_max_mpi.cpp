// Copyright 2020 Mikhail Baulin
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <climits>
#include <algorithm>

int* fillMatrixWithRandomNumbers(int* mat, int rows, int cols) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat[i * cols + j] = gen() % 10000;
        }
    }

    return mat;
}

int getSequentialOperations(int* mat, int rows, int cols) {
    int maxElement = INT_MIN;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (mat[i * cols + j] > maxElement) {
                maxElement = mat[i * cols + j];
            }
        }
    }

    return maxElement;
}

int getParallelOperations(int* mat, int rows, int cols) {
    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int delta = rows / size;

    if (rank == 0) {
        for (int proc = 1; proc < size; proc++) {
            MPI_Send(&mat[proc * delta * cols], delta * cols,
                MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    }

    int* local_mat = new int[delta * cols];

    if (rank == 0) {
        local_mat = mat;
    } else {
        MPI_Status status;
        MPI_Recv(local_mat, delta * cols, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int global_max = 0;
    int local_max = getSequentialOperations(local_mat, delta, cols);

    if (rank == 0 && rows > size * delta) {
        int local_max_tail;
        local_mat = &mat[size * delta * cols];
        local_max_tail = getSequentialOperations(local_mat, rows - (size * delta), cols);
        local_max = (local_max > local_max_tail) ? local_max : local_max_tail;
    }

    MPI_Op op_code = MPI_MAX;
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return global_max;
}
