// Copyright 2020 Kustova Anastasiya
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./max_vector.h"
TEST(Parallel_Operations_MPI, can_count_max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    const int size = 100;
    if (rank == 0) {
        vec = getRandomVector(size);
    }
    ASSERT_NO_THROW(getParallelOperations(vec));
}

TEST(Parallel_Operations_MPI, find_max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    const int size = 100;

    if (rank == 0) {
        vec = getRandomVector(size);
    }

    int global_max;
    global_max = getParallelOperations(vec);

    if (rank == 0) {
        int reference_max = getSequentialOperations(vec);
        ASSERT_EQ(reference_max, global_max);
    }
}


TEST(Parallel_Operations_MPI, can_find_local_max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec = {3, 4, 9, -6};
    if (rank == 0) {
        int reference_max = getSequentialOperations(vec);
        ASSERT_EQ(9, reference_max);
    }
}


TEST(Parallel_Operations_MPI, can_gen_random_vector) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int size = 100;

    if (rank == 0) {
        ASSERT_NO_THROW(getRandomVector(size));
    }
}

TEST(Parallel_Operations_MPI, throw_gen_vector_with_negative_length) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int size = -100;

    if (rank == 0) {
        ASSERT_ANY_THROW(getRandomVector(size));
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
