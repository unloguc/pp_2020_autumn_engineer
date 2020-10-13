// Copyright 2020 Khismatulina Karina
#include <mpi.h>
#include <vector>
#include <random>
#include <cassert>
#include "../../../modules/task_1/khismatulina_k_max_column_sum/matrix_sum_column.h"



std::vector<int> getMatrix(int line, int column) {
    assert(line > 1 && column > 1);
    std::random_device rd;
    std::mt19937 mersenne(rd);
    std::uniform_int_distribution<> dist(0, 100);
    std::vector<int> matrix(line * column);
    for (int i = 0; i < line * column; ++i) {
        matrix[i] = rd();
    }
    return matrix;
}

std::vector<int> transposedMatrix(std::vector<int> matrix, int line, int column)
{
    assert(matrix.size() != 0 && line != 0 && column != 0);
    std::vector<int> transMatrix(line * column);
    for (int i = 0; i < line; ++i) {
        for (int j = 0; j < column; ++j) {
            transMatrix[j * line + i] = matrix[i * column + j];
        }
    }
    return transMatrix;
}

int getMaxColumnSumSequential(std::vector<int> matrix, int line, int column) {

    std::vector<int> transMatrix = transposedMatrix(matrix, line, column);
    int currentSum = 0;
    int maxSum = 0;
    int k = 0;
    for (int i = 1; i < column + 1; ++i) {
        while (k != i * line) {
            currentSum += transMatrix[k];
            k += 1;
        }
        if (currentSum > maxSum) {
            maxSum = currentSum;
        }
        currentSum = 0;
    }
    return maxSum;
}

int getMaxColumnSumParallel(std::vector<int> matrix, int line, int column) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //number of process
    MPI_Comm_size(MPI_COMM_WORLD, &size); //kolichestvo of process
    MPI_Status status;
    std::vector<int> allSum(column);

    const int delta = column / size;
    int remainder = column - size * delta;

    std::vector<int> transMatrix;
    //send to slave proccess
    if (rank == 0) {
        transMatrix.resize(line * column);
        transMatrix = transposedMatrix(matrix, line, column);

        for (int proc = 1; proc < size; proc++) {
            MPI_Send(&transMatrix[proc * delta * line + remainder], delta * line,
                MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    }

    //receive from master proccess
    int* localVec;
    if (rank == 0) {
        localVec = &transMatrix[0];
    } else {
        localVec = new int[delta * line];
        MPI_Status status;
        MPI_Recv(localVec, delta * line, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int verySuperMax;
    int localMax = INT_MIN;
    MPI_Op op_code = MPI_MAX;
    if (rank == 0) {
        for (int i = 0; i < delta + remainder; ++i) {
            int sum = 0;
            for (int j = 0; j < line; ++j) {
                sum += localVec[i * line + j];
            }
            if (sum > localMax) localMax = sum;
        }
        MPI_Reduce(&localMax, &verySuperMax, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    } else {
        for (int i = 0; i < delta; ++i) {
            int sum = 0;
            for (int j = 0; j < line; ++j) {
                sum += localVec[i * column + j];
            }
            if (sum > localMax) localMax = sum;
        }
        MPI_Reduce(&localMax, &verySuperMax, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    }
    return verySuperMax;
}

