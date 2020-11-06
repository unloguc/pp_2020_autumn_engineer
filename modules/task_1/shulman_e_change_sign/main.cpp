// Copyright 2020 Shulman Egor
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./change_sign.h"

TEST(Parallel_Operations_MPI, Size_Vector_0) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<int> vector;
    const int size_vector = 0;

    if (rank == 0)
        vector = RandomVector(size_vector);

    int count = ParallelPart(vector, size_vector);

    if (rank == 0) {
        int test_count = SequentialPart(vector, size_vector);
        ASSERT_EQ(test_count, count);
    }
}

TEST(Parallel_Operations_MPI, Test_One_Many) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<int> vector;
    const int size_vector = 100;

    if (rank == 0)
        vector = RandomVector(size_vector);

    int count = ParallelPart(vector, size_vector);

    if (rank == 0) {
        int test_count = SequentialPart(vector, size_vector);
        ASSERT_EQ(test_count, count);
    }
}

TEST(Parallel_Operations_MPI, Test_All_Positive) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int size_vector = 100;
    std::vector<int> vector(size_vector);

    if (rank == 0) {
        for (int i = 0; i < size_vector; i++)
        vector[i] = i;
    }

    int count = ParallelPart(vector, size_vector);

    if (rank == 0) {
        ASSERT_EQ(0, count);
    }
}

TEST(Parallel_Operations_MPI, Test_All_Negative) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int size_vector = 100;
    std::vector<int> vector(size_vector);

    if (rank == 0) {
        for (int i = 0; i < size_vector; i++)
        vector[i] = -1 - i;
    }

    int count = ParallelPart(vector, size_vector);

    if (rank == 0) {
        ASSERT_EQ(0, count);
    }
}

TEST(Parallel_Operations_MPI, Test_Short_Vector) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int size_vector = 2;
    std::vector<int> vector(size_vector);

    if (rank == 0) {
        vector[0] = -1;
        vector[1] = 1;
    }

    int count = ParallelPart(vector, size_vector);

    if (rank == 0) {
        ASSERT_EQ(1, count);
    }
}

TEST(Parallel_Operations_MPI, Test_Ready_Set) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int size_vector = 10;
    std::vector<int> vector(size_vector);

    if (rank == 0) {
        vector[0] = -1;
        vector[1] = 0;
        vector[2] = -15;
        vector[3] = 23;
        vector[4] = 0;
        vector[5] = 0;
        vector[6] = 17;
        vector[7] = -2;
        vector[8] = -1;
        vector[9] = 100;
    }

    int count = ParallelPart(vector, size_vector);

    if (rank == 0) {
        ASSERT_EQ(5, count);
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
