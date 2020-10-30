// Copyright 2020 Evseev Alexander
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <random>
#include "./close_vec_el.h"

TEST(Test_close_el_on_vec_MPI, Test_close_one_el) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> v;
    v = { 2, 4, 8, 12, 20, 30 };
    if (rank == 0) {
        int different = getSequentialOperations(v);
        ASSERT_EQ(different, 2);
    }
}

TEST(Test_close_el_on_vec_MPI, Test_Close_1Element) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> v;
    v = { 2 };
    if (rank == 0) {
        int different = getSequentialOperations(v);
        ASSERT_EQ(different, 0);
    }
}
TEST(Test_close_el_on_vec_MPI, Test_small_size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> v;
    v = getRandomVector(1);
    int different = getParallelOperations(v);
    if (rank == 0) {
        ASSERT_EQ(different, getSequentialOperations(v));
    }
}

TEST(Test_close_el_on_vec_MPI, Test_50_size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> v;
    v = getRandomVector(50);
    int different = getParallelOperations(v);
    if (rank == 0) {
        ASSERT_EQ(different, getSequentialOperations(v));
    }
}


TEST(Test_close_el_on_vec_MPI, Test_100_size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> v;
    v = getRandomVector(50);
    int different = getParallelOperations(v);
    if (rank == 0) {
        ASSERT_EQ(different, getSequentialOperations(v));
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
