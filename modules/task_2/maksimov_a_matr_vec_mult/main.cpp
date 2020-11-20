// Copyright 2020 Maksimov Andrey
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>

#include "../../modules/task_2/maksimov_a_matr_vec_mult/matr_vec_mult.h"

#define MAX_GEN 100

TEST(Parallel_Operations_MPI, Can_Generate_Vector) {
    const int vecSize = 40;

    int* vec = getRandomVector(vecSize);
    for (int i = 0; i < vecSize; i++) {
        ASSERT_LE(vec[i], MAX_GEN);
        ASSERT_GT(vec[i], -1);
    }
    delete[] vec;
}

TEST(Parallel_Operations_MPI, Test_Negative_VecSize) {
    const int vecSize = -10;

    ASSERT_ANY_THROW(getRandomVector(vecSize););
}

TEST(Parallel_Operations_MPI, Can_Generate_Matrix) {
    const int rows = 20;
    const int columns = 10;

    int** matr = getRandomMatrix(rows, columns);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            ASSERT_LE(matr[i][j], MAX_GEN);
            ASSERT_GT(matr[i][j], -1);
        }
    }
    for (int i = 0; i < rows; i++) {
        delete[] matr[i];
    }
    delete[] matr;
}

TEST(Parallel_Operations_MPI, Test_VecSize_Not_Equal_Columns) {
    const int rows = 4;
    const int columns = 5;

    const int vecSize = 14;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int** matr;
    int* vec;
    if (rank == 0) {
        matr = getRandomMatrix(rows, columns);
        vec = getRandomVector(vecSize);
    }
    ASSERT_ANY_THROW(multiplyMatrixByVector(
        matr, rows, columns, vec, vecSize));
    ASSERT_ANY_THROW(multiplyMatrixByVectorNotParall(
        matr, rows, columns, vec, vecSize));
    if (rank == 0) {
        delete[] vec;

        for (int i = 0; i < rows; i++) {
            delete[] matr[i];
        }
        delete[] matr;
    }
}

TEST(Parallel_Operations_MPI, Test_MultiplyNonParall) {
    const int rows = 3;
    const int columns = 2;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int** matr;
    int* vec;
    if (rank == 0) {
        matr = new int*[rows];
        matr[0] = new int[] { 1, 2 };
        matr[1] = new int[] { 2, 3 };
        matr[2] = new int[] { 3, 4 };
        vec = new int[] {3, 4};

        int* multiplyNotParall
            = multiplyMatrixByVectorNotParall(matr, rows, columns, vec, columns);
        ASSERT_EQ(11, multiplyNotParall[0]);
        ASSERT_EQ(18, multiplyNotParall[1]);
        ASSERT_EQ(25, multiplyNotParall[2]);

        delete[] multiplyNotParall;
        delete[] vec;
        for (int i = 0; i < rows; i++) {
            delete[] matr[i];
        }
        delete[] matr;
    }
}

TEST(Parallel_Operations_MPI, Test_Size_Tiny) {
    const int rows = 1;
    const int columns = 1;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int** matr;
    int* vec;
    if (rank == 0) {
        matr = getRandomMatrix(rows, columns);
        vec = getRandomVector(columns);
    }
    int* multiply = multiplyMatrixByVector(
        matr, rows, columns, vec, columns);
    if (rank == 0) {
        int* multiplyNotParall = multiplyMatrixByVectorNotParall(
            matr, rows, columns, vec, columns);
        for (int i = 0; i < rows; i++) {
            ASSERT_EQ(multiplyNotParall[i], multiply[i]);
        }

        delete[] vec;

        for (int i = 0; i < rows; i++) {
            delete[] matr[i];
        }
        delete[] matr;
        delete[] multiplyNotParall;
    }
    delete[] multiply;
}

TEST(Parallel_Operations_MPI, Test_Size_Medium) {
    const int rows = 4;
    const int columns = 5;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int** matr;
    int* vec;
    if (rank == 0) {
        matr = getRandomMatrix(rows, columns);
        vec = getRandomVector(columns);
    }
    int* multiply = multiplyMatrixByVector(
        matr, rows, columns, vec, columns);
    if (rank == 0) {
        int* multiplyNotParall = multiplyMatrixByVectorNotParall(
            matr, rows, columns, vec, columns);
        for (int i = 0; i < rows; i++) {
            ASSERT_EQ(multiplyNotParall[i], multiply[i]);
        }

        delete[] vec;

        for (int i = 0; i < rows; i++) {
            delete[] matr[i];
        }
        delete[] matr;
        delete[] multiplyNotParall;
    }
    delete[] multiply;
}

TEST(Parallel_Operations_MPI, Test_Size_Huge) {
    const int rows = 9;
    const int columns = 7;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int** matr;
    int* vec;
    if (rank == 0) {
        matr = getRandomMatrix(rows, columns);
        vec = getRandomVector(columns);
    }
    int* multiply = multiplyMatrixByVector(
        matr, rows, columns, vec, columns);
    if (rank == 0) {
        int* multiplyNotParall = multiplyMatrixByVectorNotParall(
            matr, rows, columns, vec, columns);
        for (int i = 0; i < rows; i++) {
            ASSERT_EQ(multiplyNotParall[i], multiply[i]);
        }

        delete[] vec;

        for (int i = 0; i < rows; i++) {
            delete[] matr[i];
        }
        delete[] matr;
        delete[] multiplyNotParall;
    }
    delete[] multiply;
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);

    return RUN_ALL_TESTS();
}
