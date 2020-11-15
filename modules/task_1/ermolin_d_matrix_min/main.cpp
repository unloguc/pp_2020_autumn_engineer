// Copyright 2020 Ermolin Dmitry
#include <mpi.h>
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./matrix_min.h"

TEST(Parallel_Matrix_Max_MPI, Matrix_0x0) {
    int PrRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &PrRank);
    int rows = 0; int colomns = 0;
    std::vector<int> matrix;
    if (PrRank == 0)
        matrix = fillMatrixRandom(rows, colomns);
    ASSERT_ANY_THROW(MinParallel(matrix, rows, colomns));
}

TEST(Parallel_Matrix_Max_MPI, Matrix_100x1) {
    int PrRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &PrRank);
    int rows = 100, colomns = 1;
    std::vector<int> matrix;
    if (PrRank == 0)
         matrix = fillMatrixRandom(rows, colomns);
    int parMin = MinParallel(matrix, rows, colomns);
    int seqMin;
    if (PrRank == 0) {
        seqMin = MinSequential(matrix);
    }
    ASSERT_EQ(seqMin, parMin);
}

TEST(Parallel_Matrix_Max_MPI, Matrix_1x100) {
    int PrRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &PrRank);
    int rows = 1, colomns = 100;
    std::vector<int> matrix;
    if (PrRank == 0)
        matrix = fillMatrixRandom(rows, colomns);
    int parMin = MinParallel(matrix, rows, colomns);
    int seqMin;
    if (PrRank == 0) {
        seqMin = MinSequential(matrix);
    }
    ASSERT_EQ(seqMin, parMin);
}

TEST(Parallel_Matrix_Max_MPI, Matrix_200x200) {
    int PrRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &PrRank);
    int rows = 1, colomns = 35;
    std::vector<int> matrix;
    if (PrRank == 0)
        matrix = fillMatrixRandom(rows, colomns);
    int parMin = MinParallel(matrix, rows, colomns);
    int seqMin;
    if (PrRank == 0) {
        seqMin = MinSequential(matrix);
    }
    ASSERT_EQ(seqMin, parMin);
}

TEST(Parallel_Matrix_Max_MPI, Matrix_300x100) {
    int PrRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &PrRank);
    int rows = 300, colomns = 100;
    std::vector<int> matrix;
    if (PrRank == 0)
        matrix = fillMatrixRandom(rows, colomns);
    int parMin = MinParallel(matrix, rows, colomns);
    int seqMin;
    if (PrRank == 0) {
        seqMin = MinSequential(matrix);
    }
    ASSERT_EQ(seqMin, parMin);
}

TEST(Parallel_Matrix_Max_MPI, Matrix_100x300) {
    int PrRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &PrRank);
    int rows = 100, colomns = 300;
    std::vector<int> matrix;
    if (PrRank == 0)
        matrix = fillMatrixRandom(rows, colomns);
    int parMin = MinParallel(matrix, rows, colomns);
    int seqMin;
    if (PrRank == 0) {
        seqMin = MinSequential(matrix);
    }
    ASSERT_EQ(seqMin, parMin);
}

int main(int argc, char* argv[]) {
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
