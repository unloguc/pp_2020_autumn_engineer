// Copyright 2020 Evseev Alexander
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./mult_of_matrix_only_a.h"

TEST(MultOfMatOnlyA, Test_Par_Mat_Equal_One_Size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int>a = { 5 }, b = { 10 }, c, d;
    if (rank == 0) {
        c = MatrixMultiplication(a, b, 1);
    }
    d = MatrixMultiplicationMPI(a, b, 1);
    if (rank == 0) {
        ASSERT_EQ(c, d);
    }
}

TEST(MultOfMatOnlyA, Test_Throw_Negative_Size_Of_Rand_Mat) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        ASSERT_ANY_THROW(random_matrix(-1));
    }
}


TEST(MultOfMatOnlyA, Multipication_Matrix_Equal_One_Size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::vector<int>a = { 5 }, b = { 10 }, c, d = { 50 };
        c = MatrixMultiplication(a, b, 1);
        ASSERT_EQ(c, d);
    }
}

TEST(MultOfMatOnlyA, Correct_Par_Multiplication_Matrix_Small_Size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int sizeMat = 10;
    std::vector<int> A = random_matrix(sizeMat);
    std::vector<int> B = random_matrix(sizeMat);
    std::vector<int> mpiRes = MatrixMultiplicationMPI(A, B, sizeMat);
    if (rank == 0) {
        std::vector<int> SeqRes = MatrixMultiplication(A, B, sizeMat);
        ASSERT_EQ(SeqRes, mpiRes);
    }
}

TEST(MultOfMatOnlyA, Correct_Par_Multiplication_Matrix_Large_Size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int sizeMat = 50;
    std::vector<int> A = random_matrix(sizeMat);
    std::vector<int> B = random_matrix(sizeMat);
    std::vector<int> mpiRes = MatrixMultiplicationMPI(A, B, sizeMat);

    if (rank == 0) {
        std::vector<int> SeqRes = MatrixMultiplication(A, B, sizeMat);
        ASSERT_EQ(SeqRes, mpiRes);
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
