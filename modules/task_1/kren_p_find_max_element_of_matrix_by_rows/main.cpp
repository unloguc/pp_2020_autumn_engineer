// Copyright 2020 Kren Polina
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./find_max_element_of_matrix_by_rows.h"

TEST(Parallel_Operations_MPI, Test_not_correct_size_of_matrix) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> result_vector;
    const int rows = 5, columns = -4;
    if (rank == 0) {
        ASSERT_ANY_THROW(result_vector = getRandomMatrix(rows, columns));
    }
}

TEST(Parallel_Operations_MPI, Test_large_matrix_parall) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> result_matrix;
    const int rows = 40, columns = 40;
    if (rank == 0) {
        result_matrix = getRandomMatrix(rows, columns);
    }
    std::vector<int> result_max;
    result_max = getMaxInRowsParall(result_matrix, rows, columns);
    if (rank == 0) {
        std::vector<int> example_max = getMaxInRows(result_matrix, rows, columns);
        ASSERT_EQ(example_max, result_max);
    }
}

TEST(Parallel_Operations_MPI, Test_correct_size_of_matrix) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> result_vector;
    const int rows = 5, columns = 4;
    if (rank == 0) {
        ASSERT_NO_THROW(getRandomMatrix(rows, columns));
    }
}
TEST(Parallel_Operations_MPI, Test_Correct_Solve) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int rows = 3, columns = 3;
    if (rank == 0) {
        std::vector<int> result_vector = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::vector<int> result_max = getMaxInRows(result_vector, rows, columns);
        std::vector<int> example_vec = { 3, 6, 9 };
        ASSERT_EQ(example_vec, result_max);
    }
}

TEST(Parallel_Operations_MPI, Test_small_matrix_parall) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> result_matrix;
    const int rows = 20, columns = 20;
    if (rank == 0) {
        result_matrix = getRandomMatrix(rows, columns);
    }
    std::vector<int> result_max;
    result_max = getMaxInRowsParall(result_matrix, rows, columns);
    if (rank == 0) {
        std::vector<int> example_max = getMaxInRows(result_matrix, rows, columns);
        ASSERT_EQ(example_max, result_max);
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
