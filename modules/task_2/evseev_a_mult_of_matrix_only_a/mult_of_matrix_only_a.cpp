// Copyright 2020 Evseev Alexander
#include <mpi.h>
#include <random>
#include <ctime>
#include <vector>
#include <iostream>
#include <utility>
#include "../../../modules/task_2/evseev_a_mult_of_matrix_only_a/mult_of_matrix_only_a.h"


std::vector<int> random_matrix(int size) {
    if (size <= 0)
        throw "Invalid size of the matrix";

    std::mt19937 generate;
    generate.seed(static_cast<unsigned int>(time(0)));

    std::vector <int> m(size * size);
    for (auto i = 0; i < size * size; i++)
        m[i] = generate() % 10;

    return m;
}

std::vector<int> MatrixMultiplication(const std::vector<int>& A, const std::vector<int>&B, int Size) {
    int i, j, k;
    std::vector<int> C(Size * Size);
    for (i = 0; i < Size; i++) {
        for (j = 0; j < Size; j++) {
            C[i * Size + j] = 0;
            for (k = 0; k < Size; k++) {
                C[i * Size + j] += A[i * Size + k] * B[k * Size + j];
            }
        }
    }
    return C;
}

std::vector<int>  MatrixMultiplicationMPI(std::vector<int> A, std::vector<int> B, int matSize) {
    int ProcSize, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int d = matSize / ProcSize;
    int rest = matSize % ProcSize;
    std::vector<int> vec(d * matSize);
    std::vector<int> C;
    if (ProcRank == 0) {
        C.resize(matSize*matSize);
    }

    if (ProcRank == 0) {
        if (rest != 0) {
            vec.resize(matSize * d + rest * matSize);
        }
        if (d != 0) {
            for (int proc = 1; proc < ProcSize; proc++) {
                MPI_Send(&A[0] + proc * d * matSize + rest * matSize,
                    d * matSize, MPI_INT, proc, 1, MPI_COMM_WORLD);
            }
        }
    }

    if (ProcRank == 0) {
        if (rest != 0) {
            for (int i = 0; i < matSize * d + matSize * rest; i++) {
                vec[i] = A[i];
            }
        } else {
            for (int i = 0; i < matSize * d; i++) {
                vec[i] = A[i];
            }
        }
    } else {
        MPI_Status status;
        if (d != 0) {
            MPI_Recv(&vec[0], d * matSize, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        }
    }

    std::vector<int> loc(matSize * d);

    if (ProcRank == 0) {
        if (rest != 0) {
            loc.resize(matSize * d + matSize * rest);
        }

        for (auto i = 0; i < d + rest; i++) {
            for (auto j = 0; j < matSize; j++) {
                loc[i * matSize + j] = 0;
                for (auto k = 0; k < matSize; k++) {
                    loc[i * matSize + j] += vec[i * matSize + k] * B[k * matSize + j];
                }
            }
        }
    } else {
        for (auto i = 0; i < d; i++) {
            for (auto j = 0; j < matSize; j++) {
                loc[i * matSize + j] = 0;
                for (auto k = 0; k < matSize; k++) {
                    loc[i * matSize + j] += vec[i * matSize + k] * B[k * matSize + j];
                }
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (ProcRank == 0) {
        if (d != 0) {
            MPI_Status status;
            for (int proc = 1; proc < ProcSize; proc++) {
                MPI_Recv(&C[0] + proc * d * matSize + rest * matSize,
                    d * matSize, MPI_INT, proc, 2, MPI_COMM_WORLD, &status);
            }
        }
        if (rest != 0) {
            for (int i = 0; i < matSize * (d + rest); i++) {
                C[i] = loc[i];
            }
        } else {
            for (int i = 0; i < matSize * d; i++) {
                C[i] = loc[i];
            }
        }
    } else {
        if (d != 0) {
            MPI_Send(&loc[0], d * matSize, MPI_INT, 0, 2, MPI_COMM_WORLD);
        }
    }

    return C;
}



