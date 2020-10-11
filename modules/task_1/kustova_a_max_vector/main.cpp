// Copyright 2020 Kustova Anastasiya
/*#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include "./max_vector.h"


TEST(Parallel_Operations_MPI, Test_can_count_Max) {
    std::cout << 1;
    int rank;
    std::cout << 2;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::cout << 3;
    std::vector<int> global_vec;
    std::cout << 4;
    const int count_size_vector = 100;
    std::cout << 5;
    if (rank == 0) {
    std::cout << 6;
        global_vec = getRandomVector(count_size_vector);
    std::cout << 7;
    }
    std::cout << 8;
    ASSERT_NO_THROW(getParallelOperations(global_vec, count_size_vector, "max"));
}

TEST(Parallel_Operations_MPI, Test_Max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec;
    const int count_size_vector = 100;

    if (rank == 0) {
        global_vec = getRandomVector(count_size_vector);
    }

    int global_max;
    global_max = getParallelOperations(global_vec, count_size_vector, "max");

    if (rank == 0) {
        int reference_max = getSequentialOperations(global_vec, "max");
        ASSERT_EQ(reference_max, global_max);
    }
}

TEST(Parallel_Operations_MPI, Test_can_find_local_max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec = {3, 4, 9, -6};
    if (rank == 0) {
        int reference_max = getSequentialOperations(global_vec, "max");
        ASSERT_EQ(9, reference_max);
    }
}


TEST(Parallel_Operations_MPI, Test_can_gen_random_vector) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec1;
    std::vector<int> global_vec2;
    const int count_size_vector = 100;

    if (rank == 0) {
        ASSERT_NO_THROW(getRandomVector(count_size_vector));
    }
}

TEST(Parallel_Operations_MPI, Test_throw_empty_vector) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec;
    ASSERT_ANY_THROW(getParallelOperations(global_vec, 0, "max"));
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
}*/
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./max_vector.h"
TEST(Parallel_Operations_MPI, Test_can_count_Max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec;
    if (rank == 0) {
        global_vec = getRandomVector();
    }
    ASSERT_NO_THROW(getParallelOperations(global_vec));
}

TEST(Parallel_Operations_MPI, Test_Max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec;
    if (rank == 0) {
        global_vec = getRandomVector();
    }

    int global_max;
    global_max = getParallelOperations(global_vec);

    if (rank == 0) {
        int reference_max = getSequentialOperations(global_vec);
        ASSERT_EQ(reference_max, global_max);
    }
}


TEST(Parallel_Operations_MPI, Test_can_find_local_max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> global_vec = {3, 4, 9, -6};
    if (rank == 0) {
        int reference_max = getSequentialOperations(global_vec);
        ASSERT_EQ(9, reference_max);
    }
}


TEST(Parallel_Operations_MPI, Test_can_gen_random_vector) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        ASSERT_NO_THROW(getRandomVector());
    }
}

TEST(Parallel_Operations_MPI, Test_can_gen_not_empty_vector) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    if (rank == 0) {
        vec = getRandomVector();
        ASSERT_TRUE(vec.size() != 0);
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
