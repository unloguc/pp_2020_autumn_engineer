// Copyright 2020 Vorobev Leonid
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "../../modules/task_1/vorobev_l_mid/mid.h"

TEST(Parallel_Operations_MPI, Vector) {
    const int sizevec = 10;

    std::vector<int> vec = randvector(sizevec);
    ASSERT_EQ(static_cast<int>(vec.size()), sizevec);
}

TEST(Parallel_Operations_MPI, Test_Minus_SizeVec) {
    const int sizevec = -10;

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

TEST(Parallel_Operations_MPI, Test_Size_Low) {
    const int sizevec = 10;
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
TEST(Parallel_Operations_MPI, Test_Size_Mid) {
    const int sizevec = 100;

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

TEST(Parallel_Operations_MPI, Test_Size_Many) {
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
