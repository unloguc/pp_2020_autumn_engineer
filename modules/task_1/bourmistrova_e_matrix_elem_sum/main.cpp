// Copyright 2020 Ekaterina Burmistrova
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>

#include <vector>

#include "./mpi_fun.h"

TEST(Parallel_Operations_MPI, Test_Sum) {
    int mynode;
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
    const int rows = 100;
    const int columns = 100;
    std::vector<int> matrix(rows*columns);
    if (mynode == 0) {
        matrix = getRandomMatrix(rows, columns);
    }

    int par_sum = Parallel_method(matrix, rows, columns);

    if (mynode == 0) {
        int ref_sum = 0;
        ref_sum = Sequential_method(matrix);
        ASSERT_EQ(ref_sum, par_sum);
    }
}
int main(int argc, char **argv) {
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
