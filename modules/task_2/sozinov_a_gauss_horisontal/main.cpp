// Copyright 2020 Sozinov Alex
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <mpi.h>
#include <vector>
#include "./gauss_horisontal.h"

TEST(Parallel_Operations_MPI, test_find_seq) {
    std::vector<double> coefs{ 1, 3, 2,
                               2, 7, 5,
                               1, 4, 6 };
    std::vector<double> pPart{ 1, 18, 26 };
    ASSERT_NO_THROW(GetSequentialSolution(coefs, pPart, 3));
}

TEST(Parallel_Operations_MPI, test_find_par) {
    std::vector<double> coefs{ 1, 3, 2,
                               2, 7, 5,
                               1, 4, 6 };
    std::vector<double> pPart{ 1, 18, 26 };
    ASSERT_NO_THROW(GetParalSolution(coefs, pPart, 3));
}

TEST(Parallel_Operations_MPI, test_sol_1) {
    std::vector<double> coefs{ -1, 4, 8, 6, 4,
                               -5, 2, -8, 5, 4,
                               4, -5, 1, 5, 10,
                               -4, 3, 1, -7, 5,
                               5, 10, 7, -4, -3 };
    std::vector<double> coefs2(coefs);
    std::vector<double> pPart{ 6, -5, 3, 5, 3 };
    std::vector<double> pPart2(pPart);
    int size = pPart.size();
    double start1 = MPI_Wtime();

    std::vector<double> s = GetSequentialSolution(coefs, pPart, size);

    double end1 = MPI_Wtime();
    double start2 = MPI_Wtime();

    std::vector<double> p = GetParalSolution(coefs2, pPart2, size);
    double end2 = MPI_Wtime();
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0) {
        printf("Matrix 5x5\nSequential - %f\nParallel - %f", end1 - start1, end2 - start2);
        ASSERT_EQ(s, p);
    }
}

TEST(Parallel_Operations_MPI, test_sol_2) {
    std::vector<double> coefs{ 1, 3, 2,
                              2, 7, 5,
                              1, 4, 6 };
    std::vector<double> coefs2(coefs);
    std::vector<double> pPart{ 1, 18, 26 };
    std::vector<double> pPart2(pPart);
    int size = pPart.size();
    double start1 = MPI_Wtime();

    std::vector<double> s = GetSequentialSolution(coefs, pPart, size);

    double end1 = MPI_Wtime();
    double start2 = MPI_Wtime();

    std::vector<double> p = GetParalSolution(coefs2, pPart2, size);
    double end2 = MPI_Wtime();
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0) {
        printf("Matrix 4x4\nSequential - %f\nParallel - %f", end1 - start1, end2 - start2);
        ASSERT_EQ(s, p);
    }
}

TEST(Parallel_Operations_MPI, test_sol_3) {
    std::vector<double> coefs{ 3, -2, 5, 1 };
    std::vector<double> coefs2(coefs);
    std::vector<double> pPart{ -6, 3 };
    std::vector<double> pPart2(pPart);
    int size = pPart.size();
    double start1 = MPI_Wtime();

    std::vector<double> s = GetSequentialSolution(coefs, pPart, size);

    double end1 = MPI_Wtime();
    double start2 = MPI_Wtime();

    std::vector<double> p = GetParalSolution(coefs2, pPart2, size);
    double end2 = MPI_Wtime();
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0) {
        printf("Matrix 2x2\nSequential - %f\nParallel - %f", end1 - start1, end2 - start2);
        ASSERT_EQ(s, p);
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
