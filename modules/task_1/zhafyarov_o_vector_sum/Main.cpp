  // Copyright 2020 Zhafyarov Oleg
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./vector_sum.h"

TEST(Parallel_Operations_MPI, Test_Sum) {
    int Size = 1000;
    std::vector<int> vec(Size);
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == 0) {
        vec = getRandomVector(Size);
    }

    int parallel_sum = getParallelOperations(vec, Size);

    if (ProcRank == 0) {
        int sequential_sum = getSequentialOperations(vec);
        ASSERT_EQ(parallel_sum, sequential_sum);
    }
}

TEST(Parallel_Operations_MPI, Test_Vector1) {
    int Size = 5;
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    int sum = 15;
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int parallel_sum = getParallelOperations(vec, Size);

    if (ProcRank == 0) {
        ASSERT_EQ(parallel_sum, sum);
    }
}

TEST(Parallel_Operations_MPI, Test_Vector2) {
    int Size = 5;
    std::vector<int> vec = { 0, 0, 0, 0, 0 };
    int sum = 3;
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int parallel_sum = getParallelOperations(vec, Size);

    if (ProcRank == 0) {
        ASSERT_NE(parallel_sum, sum);
    }
}

TEST(Parallel_Operations_MPI, Test_Vector3) {
    int Size = 5;
    std::vector<int> vec = { -1, -2, -3, -4, -5 };
    int sum = 0;
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int parallel_sum = getParallelOperations(vec, Size);

    if (ProcRank == 0) {
        ASSERT_LT(parallel_sum, sum);
    }
}

TEST(Parallel_Operations_MPI, Test_Vector4) {
    int Size = 10;
    std::vector<int> vec(Size);
    int sum = 0;
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == 0) {
        vec = getRandomVector(Size);
    }

    int parallel_sum = getParallelOperations(vec, Size);

    if (ProcRank == 0) {
        vec.pop_back();
        int sequential_sum = getSequentialOperations(vec);
        ASSERT_NE(parallel_sum, sum);
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


