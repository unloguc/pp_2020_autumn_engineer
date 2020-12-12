// Copyright 2020 Mikhail Baulin
#include <gtest/gtest.h>
#include <vector>
#include "./matrix_max_mpi.h"
#include "../../../3rdparty/gtest-mpi/linux/gtest-mpi-listener.hpp"


TEST(Parallel_Operations_MPI, Test_Max) {
    int rank;
    int rows = 50;
    int cols = 50;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* mat = new int[rows * cols];

    if (rank == 0) {
        mat = fillMatrixWithRandomNumbers(mat, rows, cols);
    }

    int global_max;
    global_max = getParallelOperations(mat, rows, cols);

    if (rank == 0) {
        int reference_max = getSequentialOperations(mat, rows, cols);
        ASSERT_EQ(reference_max, global_max);
    }
}

TEST(Parallel_Operations_MPI, Test_Max_24) {
    int rank;
    int rows = 5;
    int cols = 5;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* mat = new int[rows * cols];

    if (rank == 0) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++)
                mat[i * cols + j] = i * cols + j;
        }
    }

    int global_max;
    global_max = getParallelOperations(mat, rows, cols);

    if (rank == 0) {
        ASSERT_EQ(24, global_max);
    }
}

TEST(Parallel_Operations_MPI, Test_Max_19607) {
    int rank;
    int rows = 129;
    int cols = 152;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* mat = new int[rows * cols];

    if (rank == 0) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++)
                mat[i * cols + j] = i * cols + j;
        }
    }

    int global_max;
    global_max = getParallelOperations(mat, rows, cols);

    if (rank == 0) {
        ASSERT_EQ(19607, global_max);
    }
}

TEST(Parallel_Operations_MPI, Test_Max_55555) {
    int rank;
    int rows = 129;
    int cols = 152;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* mat = new int[rows * cols];

    if (rank == 0) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++)
                mat[i * cols + j] = i * cols + j;
        }
    }

    mat[75 * cols + 54] = 55555;

    int global_max;
    global_max = getParallelOperations(mat, rows, cols);

    if (rank == 0) {
        ASSERT_EQ(55555, global_max);
    }
}

TEST(Parallel_Operations_MPI, Test_Max_Negative) {
    int rank;
    int rows = 129;
    int cols = 152;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* mat = new int[rows * cols];

    if (rank == 0) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++)
                mat[i * cols + j] = i * cols + j - 400000;
        }
    }

    int global_max;
    global_max = getParallelOperations(mat, rows, cols);

    if (rank == 0) {
        int reference_max = getSequentialOperations(mat, rows, cols);
        ASSERT_EQ(reference_max, global_max);
    }
}

int main(int argc, char** argv) {
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
