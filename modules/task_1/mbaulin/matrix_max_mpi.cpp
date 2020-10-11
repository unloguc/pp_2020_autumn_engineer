// Copyright 2020 Mikhail Baulin
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>

int** fillMatrixWithRandomNumbers(int** mat, int rows, int cols) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat[i][j] = gen() % 10000;
        }
    }

    return mat;
}

int findMax(int** mat, int rows, int cols) {
    int maxElement = -1;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (mat[i][j] > maxElement) {
                maxElement = mat[i][j];
            }
        }
    }

    return maxElement;
}

int getParallelOperations(int** mat, int rows, int cols) {
    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int delta = rows / size;

    if (rank == 0) {
        for (int proc = 1; proc < size; proc++) {
            for (int i = proc * delta; i < (proc + 1) * delta ; i++) {
                if (i > rows) break;
                MPI_Send(mat[i], cols,
                    MPI_INT, proc, 0, MPI_COMM_WORLD);
            }
        }
    }

    int** local_mat = new int* [delta];
    for (int i = 0; i < delta; i++)
        local_mat[i] = new int[cols];

    if (rank == 0) {
        local_mat = mat;
    } else {
        for (int i = 0; i < delta; i++) {
            MPI_Status status;
            MPI_Recv(local_mat[i], cols, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
    }

    int global_max = 0;
    int local_max = findMax(local_mat, delta, cols);

    if (rank == 0 && rows > size * delta) {
        int local_max_tail;
        local_mat = &mat[size * delta];
        local_max_tail = findMax(local_mat, rows - (size * delta), cols);
        local_max = (local_max > local_max_tail) ? local_max : local_max_tail;
    }

    MPI_Op op_code = MPI_MAX;
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return global_max;
}
