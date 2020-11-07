// Copyright 2020 Ermolin Dmitry
#include "../../../modules/task_1/ermolin_d_matrix_min/matrix_min.h"
#include <mpi.h>
#include <vector>
#include <random>
#include <algorithm>

std::vector<int> fillMatrixRandom(int rows, int columns) {
    std::mt19937 gen;
    std::random_device seed;
    gen.seed(static_cast<unsigned int>(seed()));
    std::vector<int> matrix(rows * columns);
    for (int i = 0; i < rows * columns; i++) {
        matrix[i] = gen() % 100;
    }
    return matrix;
}

int MinSequential(const std::vector<int>& matrix) {
    if (matrix.size() == 0) {
       throw "Incorrect matrix size.";
    }
    int result = *std::min_element(matrix.begin(), matrix.end());
    return result;
}

int MinParallel(const std::vector<int>& matrix, int rows, int columns) {
    int PrNum, PrRank;
    int MatrixSize = rows * columns;
    if (MatrixSize == 0) {
       throw "Incorrect matrix size.";
    }

    MPI_Comm_size(MPI_COMM_WORLD, &PrNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &PrRank);

    const int delta = MatrixSize % PrNum;
    const int epsilon = MatrixSize / PrNum;

    int localSize;
    if (PrRank == 0) {
        for (int i = 1; i < PrNum; i++)
            MPI_Send(&matrix[0] + delta + i * epsilon, epsilon, MPI_INT, i, 0, MPI_COMM_WORLD);
        localSize = delta + epsilon;
    } else {
            localSize = epsilon;
    }

    std::vector<int> localMatrix(localSize);
    if (PrRank == 0) {
        localMatrix = std::vector<int>(matrix.begin(), matrix.begin() + localSize);
    } else {
        MPI_Status status;
        MPI_Recv(&localMatrix[0], localSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int MatrixMin;
    int localMin = MinSequential(localMatrix);
    MPI_Reduce(&localMin, &MatrixMin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    return MatrixMin;
}
