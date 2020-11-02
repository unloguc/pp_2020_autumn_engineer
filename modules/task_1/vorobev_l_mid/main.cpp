// Copyright 2020 Vorobev Leonid
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>

#include <vector>

#include "../../modules/task_1/vorobjev_l_mid/mid.h"

TEST(Parallel_Operations_MPI, Can_Generate_Vector) {
    const int sizevec = 10;

    std::vector<int> vec = randvector(sizevec);
    ASSERT_EQ(static_cast<int>(vec.size()), sizevec);
}

TEST(Parallel_Operations_MPI, Test_Minus_Sizevec) {
    const int sizevec = -8;

    ASSERT_ANY_THROW(randvector(sizevec));
}

TEST(Parallel_Operations_MPI, Test_Size_1) {
    const int sizevec = 1;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    if (rank == 0) {
        vec = randvector(sizevec);
    }
    int avg = avgvectorpar(vec, sizevec);
    if (rank == 0) {
        int avgvec = avgvector(vec, sizevec);
        ASSERT_EQ(avgvec, avg);
    }
}

TEST(Parallel_Operations_MPI, Test_Size_low) {
    const int sizevec = 8;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    if (rank == 0) {
        vec = randvector(sizevec);
    }
    int avg = avgvectorpar(vec, sizevec);
    if (rank == 0) {
        int avgvec = avgvector(vec, sizevec);
        ASSERT_EQ(avgvec, avg);
    }
}
TEST(Parallel_Operations_MPI, Test_Size_mid) {
    const int sizevec = 80;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    if (rank == 0) {
        vec = randvector(sizevec);
    }
    int avg = avgvectorpar(vec, sizevec);
    if (rank == 0) {
        int avgvec = avgvector(vec, sizevec);
        ASSERT_EQ(avgvec, avg);
    }
}

TEST(Parallel_Operations_MPI, Test_Size_many) {
    const int sizevec = 1000;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    if (rank == 0) {
        vec = randvector(sizevec);
    }
    int avg = avgvectorpar(vec, sizevec);
    if (rank == 0) {
        int avgvec = avgvector(vec, sizevec);
        ASSERT_EQ(avgvec, avg);
    }
}

int main(int argc, char* argv[]) {
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
