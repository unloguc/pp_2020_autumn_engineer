  // Copyright 2020 Maksimov Andrey

#include <mpi.h>
#include <time.h>
#include <random>

#include "../../modules/task_2/maksimov_a_matr_vec_mult/matr_vec_mult.h"

#define MAX_GEN 100

int* getRandomVector(int size, int seedMod) {
    if (size < 1) {
        throw "size < 1 error";
    }

    std::mt19937 gen;
    gen.seed(time(0) + seedMod);

    int* vec = new int[size];
    for (int i = 0; i < size; i++) {
        vec[i] = gen() % MAX_GEN;
    }
    return vec;
}

int** getRandomMatrix(int rows, int columns) {
    int** matr = new int*[rows];
    for (int i = 0; i < rows; i++) {
        matr[i] = getRandomVector(columns, i);
    }
    return matr;
}

int* multiplyMatrixByVectorNotParall(
    int** matr, int rows, int columns,
    int* vec, int vecSize) {

    if (columns != vecSize)
        throw "columns != vecSize error";

    int* result = new int[rows];
    int sum = 0;
    for (int i = 0; i < rows; i++) {
        sum = 0;
        for (int j = 0; j < columns; j++) {
            sum += matr[i][j] * vec[j];
        }
        result[i] = sum;
    }

    return result;
}

int* multiplyMatrixByVector(
    int** matr, int rows, int columns,
    int* vec, int vecSize) {


    if (columns != vecSize)
        throw "columns != vecSize error";

    MPI_Status mpiStatus;
    int procNum, procRank, sum, rowsPart, packedDataSize;

    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    rowsPart = rows / procNum;
    packedDataSize = columns + rowsPart * columns;

    int* packedData = new int[packedDataSize];

    if (procRank == 0) {
        for (int i = 0; i < columns; i++) {
            packedData[i] = vec[i];
        }
        for (int currProc = 1; currProc < procNum; currProc++) {
            for (int row = 0; row < rowsPart; row++) {
                for (int column = 0; column < columns; column++) {
                    packedData[columns + row * columns + column]
                        = matr[currProc * rowsPart + row][column];
                }
            }
            MPI_Send(&packedData[0], packedDataSize, MPI_INT, currProc, 0, MPI_COMM_WORLD);
        }
    }

    if (procRank != 0) {
        MPI_Recv(&packedData[0], packedDataSize, MPI_INT, 0,
            0, MPI_COMM_WORLD, &mpiStatus);
    }

    int* returnVec = new int[rows];

    int* resultVec = new int[rowsPart];
    if (procRank == 0) {
        for (int row = 0; row < rowsPart; row++) {
            sum = 0;
            for (int column = 0; column < columns; column++) {
                sum += matr[row][column] * vec[column];
            }
            returnVec[row] = sum;
        }
    } else {
        for (int row = 0; row < rowsPart; row++) {
            sum = 0;
            for (int column = 0; column < columns; column++) {
                sum += packedData[columns + row * columns + column] * packedData[column];
            }
            resultVec[row] = sum;
        }
    }

    delete[] packedData;

    if (procRank != 0) {
        MPI_Send(&resultVec[0], rowsPart, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (procRank == 0) {
        for (int currProc = 1; currProc < procNum; currProc++) {
            MPI_Recv(&returnVec[currProc * rowsPart], rowsPart, MPI_INT, currProc,
                0, MPI_COMM_WORLD, &mpiStatus);
        }

        for (int i = 0; i < rows % procNum; i++) {
            sum = 0;
            for (int j = 0; j < columns; j++) {
                sum += matr[rowsPart * procNum + i][j] * vec[j];
            }
            returnVec[rowsPart * procNum + i] = sum;
        }
    }

    delete[] resultVec;

    return returnVec;
}
