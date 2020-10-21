// Copyright 2020 Kochankov Ilya
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./matrix_columns_sum.h"


TEST(Parallel_Operations_MPI, parallel_sum_works) {
    Matrix(int) matrix(3);
    matrix[0] = vector<int>(4);
    vector<int> result_vector;
    vector<int> parallel_result;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        matrix = Matrix(int)({ {1, 5, 2, 3}, {1, 5, 2, 3}, {1, 5, 2, 3} });
        result_vector = vector<int>({3, 15, 6, 9});
    }

    parallel_result = parallel_sum(matrix);

    if (rank == 0) {
        EXPECT_EQ(parallel_result, result_vector);
    }
}

TEST(Parallel_Operations_MPI, sequential_operations_works) {
    Matrix(int) matrix(3);
    matrix[0] = vector<int>(4);
    vector<int> result_vector;
    vector<int> sequential_result;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        matrix = Matrix(int)({ {1, 5, 2, 3}, {1, 5, 2, 3}, {1, 5, 2, 3} });
        result_vector = vector<int>({3, 15, 6, 9});

        sequential_result = sequential_operations(matrix);

        EXPECT_EQ(sequential_result, result_vector);
    }
}

TEST(Parallel_Operations_MPI, parallel_sum_one_line) {
    Matrix(int) matrix(1);
    matrix[0] = vector<int>(4);
    vector<int> result_vector;
    vector<int> parallel_result;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        matrix = Matrix(int)({{1, 2, 3, 4}});
        result_vector = vector<int>({1, 2, 3, 4});
    }

    parallel_result = parallel_sum(matrix);

    if (rank == 0) {
        EXPECT_EQ(parallel_result, result_vector);
    }
}

TEST(Parallel_Operations_MPI, get_rand_matrix_works) {
    Matrix(int) matrix(5);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        matrix = get_rand_matrix(5, 6);

        EXPECT_EQ(static_cast<int>(matrix.size()), 5);
        for (int i = 0; i < 5; i++) {
            EXPECT_EQ(static_cast<int>(matrix[i].size()), 6);
            for (int j = 0; j < 6; j++) {
                EXPECT_GE(matrix[i][j], 0);
            }
        }
    }
}

TEST(Parallel_Operations_MPI, get_rand_matrix_negative_size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        EXPECT_ANY_THROW(auto matrix = get_rand_matrix(-5, -9));
    }
}

TEST(Parallel_Operations_MPI, parallel_sum_works_random_matrix) {
    Matrix(int) matrix(1000);
    matrix[0] = vector<int>(1000);
    vector<int> sequential_result;
    vector<int> parallel_result;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        matrix = get_rand_matrix(1000, 1000);
        sequential_result = sequential_operations(matrix);
    }

    parallel_result = parallel_sum(matrix);

    if (rank == 0) {
        EXPECT_EQ(parallel_result, sequential_result);
    }
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners &listeners =
            ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
