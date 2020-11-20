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
    int procNum, procRank, size, sum, rowsPart;

    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int* returnVec = new int[rows];

    rowsPart = rows / procNum;
    size = rowsPart * columns;

    if (procRank != 0) {
        vec = new int[columns];
    }

    MPI_Bcast(&vec[0], columns, MPI_INT, 0, MPI_COMM_WORLD);

    int* matrPart1d = new int[size];

    if (procRank == 0) {
        for (int i = 1; i < procNum; i++) {
            for (int row = 0; row < rowsPart; row++) {
                for (int column = 0; column < columns; column++) {
                    matrPart1d[row * columns + column]
                        = matr[i * rowsPart + row][column];
                }
            }
            MPI_Send(&matrPart1d[0], size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    if (procRank != 0) {
        MPI_Recv(&matrPart1d[0], size, MPI_INT, 0,
            0, MPI_COMM_WORLD, &mpiStatus);
    }

    int* resultVec = new int[rowsPart];
    if (procRank == 0) {
        for (int i = 0; i < rowsPart; i++) {
            sum = 0;
            for (int j = 0; j < columns; j++) {
                sum += matr[i][j] * vec[j];
            }
            returnVec[i] = sum;
        }
    } else {
        for (int i = 0; i < rowsPart; i++) {
            sum = 0;
            for (int j = 0; j < columns; j++) {
                sum += matrPart1d[i * columns + j] * vec[j];
            }
            resultVec[i] = sum;
        }
    }

    delete[] matrPart1d;

    if (procRank != 0) {
        MPI_Send(&resultVec[0], rowsPart, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (procRank == 0) {
        int* tempVec = new int[rowsPart];

        for (int i = 1; i < procNum; i++) {
            MPI_Recv(&tempVec[0], rowsPart, MPI_INT, i,
                0, MPI_COMM_WORLD, &mpiStatus);

            for (int j = 0; j < rowsPart; j++) {
                returnVec[i * rowsPart + j] = tempVec[j];
            }
        }

        delete[] tempVec;

        for (int i = 0; i < rows % procNum; i++) {
            sum = 0;
            for (int j = 0; j < columns; j++) {
                sum += matr[rowsPart * procNum + i][j] * vec[j];
            }
            returnVec[rowsPart * procNum + i] = sum;
        }
    }
    delete[] resultVec;
    if (procRank != 0) {
        delete[] vec;
    }

    return returnVec;
}
