// Copyright 2020 Chesnokov Artyom
#include <time.h>
#include <gtest/gtest.h>
#include <gtest-mpi-listener.hpp>
#include <random>
#include "./scatter.h"

TEST(Task_2, Test_Scatter_Naive_Works) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int test_data[1000];
    if (rank == 0) {
        std::mt19937 gen;
        gen.seed(static_cast<unsigned int>(time(0)));
        for (int i = 0; i < 1000; i++) {
            test_data[i] = gen() % 128;
        }
    }
    int delta = 1000 / size;
    int *recv1, *recv2;
    recv1 = new int[delta];
    recv2 = new int[delta];

    MPI_Scatter(test_data, delta, MPI_INT, recv1, delta, MPI_INT, 0, MPI_COMM_WORLD);
    EXPECT_NO_THROW(My_Scatter_Naive(test_data, delta, MPI_INT,
        recv2, delta, MPI_INT, 0, MPI_COMM_WORLD));

    for (int i = 0; i < delta; i++) {
        EXPECT_EQ(recv1[i], recv2[i]);
    }
}

TEST(Task_2, Test_Scatter_By_Tree_Works_With_Root_Eq_0) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int test_data[1000];
    if (rank == 0) {
        std::mt19937 gen;
        gen.seed(static_cast<unsigned int>(time(0)));
        for (int i = 0; i < 1000; i++) {
            test_data[i] = gen() % 128;
        }
    }
    int delta = 1000 / size;
    int *recv1, *recv2;
    recv1 = new int[delta];
    recv2 = new int[delta];

    MPI_Scatter(test_data, delta, MPI_INT, recv1, delta, MPI_INT, 0, MPI_COMM_WORLD);
    EXPECT_NO_THROW(My_Scatter_By_Tree(test_data, delta, MPI_INT,
        recv2, delta, MPI_INT, 0, MPI_COMM_WORLD));

    for (int i = 0; i < delta; i++) {
        EXPECT_EQ(recv1[i], recv2[i]);
    }
}

TEST(Task_2, Test_Scatter_By_Tree_Works_With_Root_Neq_0) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int test_data[1000];
    if (rank == 0) {
        std::mt19937 gen;
        gen.seed(static_cast<unsigned int>(time(0)));
        for (int i = 0; i < 1000; i++) {
            test_data[i] = gen() % 128;
        }
    }
    int delta = 1000 / size;
    int *recv1, *recv2;
    recv1 = new int[delta];
    recv2 = new int[delta];

    MPI_Scatter(test_data, delta, MPI_INT, recv1, delta, MPI_INT, size / 2, MPI_COMM_WORLD);
    EXPECT_NO_THROW(My_Scatter_By_Tree(test_data, delta, MPI_INT,
        recv2, delta, MPI_INT, size / 2, MPI_COMM_WORLD));

    for (int i = 0; i < delta; i++) {
        EXPECT_EQ(recv1[i], recv2[i]);
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
