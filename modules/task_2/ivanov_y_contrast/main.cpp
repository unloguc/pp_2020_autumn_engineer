  // Copyright 2020 Ivanov Yaroslav
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "./contrast.h"

TEST(Parallel_Operation_MPI, Test_Can_Create_Image_10x10) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int height = 10;
    int width = 10;
    std::vector<int> Arr = CreateRandomImg(height, width);
    std::vector<int> Arr2 = ParallelContrast(Arr, height, width);
    if (rank == 0) {
        std::vector<int> Arr1 = LocalContrast(Arr, height, width);
        ASSERT_EQ(Arr1, Arr2);
    }
}

TEST(Parallel_Operation_MPI, Test_Can_Create_Image_30x30) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int height = 30;
    int width = 30;
    std::vector<int> Arr = CreateRandomImg(height, width);
    std::vector<int> Arr2 = ParallelContrast(Arr, height, width);
    if (rank == 0) {
        std::vector<int> Arr1 = LocalContrast(Arr, height, width);
        ASSERT_EQ(Arr1, Arr2);
    }
}

TEST(Parallel_Operation_MPI, Test_Can_Create_Image_100x100) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int height = 100;
    int width = 100;
    std::vector<int> Arr = CreateRandomImg(height, width);
    std::vector<int> Arr2 = ParallelContrast(Arr, height, width);
    if (rank == 0) {
        std::vector<int> Arr1 = LocalContrast(Arr, height, width);
        ASSERT_EQ(Arr1, Arr2);
    }
}

TEST(Parallel_Operation_MPI, Test_Can_Create_Image_200x200) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int height = 200;
    int width = 200;
    std::vector<int> Arr = CreateRandomImg(height, width);
    std::vector<int> Arr2 = ParallelContrast(Arr, height, width);
    if (rank == 0) {
        std::vector<int> Arr1 = LocalContrast(Arr, height, width);
        ASSERT_EQ(Arr1, Arr2);
    }
}

TEST(Parallel_Operation_MPI, Test_Can_Create_Image_1000x1000) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int height = 1000;
    int width = 1000;
    std::vector<int> Arr = CreateRandomImg(height, width);
    std::vector<int> Arr2 = ParallelContrast(Arr, height, width);
    if (rank == 0) {
        std::vector<int> Arr1 = LocalContrast(Arr, height, width);
        ASSERT_EQ(Arr1, Arr2);
    }
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);
    AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());
    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}


