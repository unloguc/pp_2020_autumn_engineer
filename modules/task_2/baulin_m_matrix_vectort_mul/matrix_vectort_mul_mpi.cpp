// Copyright 2020 Mikhail Baulin
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>

std::vector<int> getMatrixWithRandomNumbers(int rows, int cols) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::uniform_int_distribution<> dist(0, 1000);
    std::vector<int> mat(rows * cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat[i * cols + j] = dist(gen) % 10000;
        }
    }

    return mat;
}

std::vector<int> getSequentialOperations(std::vector<int> mat, int rows, int cols, std::vector<int> vec) {
    std::vector<int> result(rows);

    for (int i = 0; i < rows; i++) {
        int sum = 0;
        for (int j = 0; j < cols; j++) {
            sum += mat[i * cols + j] * vec[j];
        }
        result[i] = sum;
    }
    return result;
}

std::vector<int> getParallelOperations(std::vector<int> mat, int rows, int cols, std::vector<int> vec) {
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

    std::vector<int> local_mat(1);
    if (delta > 0) {
        local_mat = std::vector<int>(delta * cols);
    }
    if (rank == 0) {
        local_mat = mat;
    } else {
        MPI_Status status;
        MPI_Recv(&local_mat[0], delta * cols, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    std::vector<int> global_result(rows);
    std::vector<int> local_result(1);
    if (delta > 0) {
        local_result = getSequentialOperations(local_mat, delta, cols, vec);
    }

    MPI_Gather(&local_result[0], delta, MPI_INT, &global_result[0], delta, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0 && rows > size * delta) {
        std::vector<int> local_result_tail;
        local_mat = std::vector<int>(local_mat.begin() + (size * delta * cols), local_mat.end());
        local_result_tail = getSequentialOperations(local_mat, rows - (size * delta), cols, vec);
        for (int i = size * delta; i < rows; ++i) {
            global_result[i] = local_result_tail[i - size * delta];
        }
    }

    return global_result;
}
