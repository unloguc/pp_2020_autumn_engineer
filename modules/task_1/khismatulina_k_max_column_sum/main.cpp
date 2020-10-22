// Copyright 2020 Khismatulina Karina
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "../../../modules/task_1/khismatulina_k_max_column_sum/matrix_sum_column.h"

TEST(matrix_summ_solumn_tests, test_1_sequential_5x5) {
    int rank;
    std::vector<int> vec{ 0,  1,  2,  3,  4,
                               5,  6,  7,  8,  9,
                               10, 11, 12, 13, 14,
                               15, 16, 17, 18, 19,
                               20, 21, 22, 23, 24 };

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        ASSERT_EQ(getMaxColumnSumSequential(vec, 5, 5), 70);
    }
}

TEST(matrix_summ_solumn_tests, test_2_sequential_100x100) {
    int rank;
    std::vector<int> vec(10000);
    for (int i = 0; i < 10000; ++i) {
        vec[i] = 1;
    }
    int res = getMaxColumnSumSequential(vec, 100, 100);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        ASSERT_EQ(res, 100);
    }
}

TEST(matrix_summ_solumn_tests, test_1_parallel_matrix_3x4_random) {
    std::vector<int> vec;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        vec = getMatrix(3, 4);
    }

    int res = getMaxColumnSumParallel(vec, 3, 4);
    if (rank == 0) {
        ASSERT_EQ(res, getMaxColumnSumSequential(vec, 3, 4));
    }
}

TEST(matrix_summ_solumn_tests, test_1_parallel_matrix_3x4_not_random) {
    std::vector<int> vec;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        vec = {1, 2,  3,  4,
               5, 6,  7,  8,
               9, 10, 11, 12};
    }

    int res = getMaxColumnSumParallel(vec, 3, 4);
    if (rank == 0) {
        ASSERT_EQ(res, getMaxColumnSumSequential(vec, 3, 4));
    }
}

TEST(matrix_summ_solumn_tests, test_2_parallel_matrix_150x150) {
    std::vector<int> vec;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        vec = getMatrix(150, 150);
    }

    int res = getMaxColumnSumParallel(vec, 150, 150);
    if (rank == 0) {
        ASSERT_EQ(res, getMaxColumnSumSequential(vec, 150, 150));
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

#ifdef _DEBUG

    int rank, temp;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    if (rank == 0) {
        std::cout << "Please input number: ";
        std::cin >> temp;
    }

    MPI_Barrier(MPI_COMM_WORLD);

#endif

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}


