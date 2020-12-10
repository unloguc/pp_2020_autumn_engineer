// Copyright 2020 Ermolin Dmitry
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "./hypercube.h"

TEST(Parallel_Operations_MPI, Create_HyperCube) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm comm = HyperCube();
    int count = DimCount(size);
    int realCount, status;

    int* dim = new int[count];
    int* period = new int[count];
    int* coord = new int[count];
    MPI_Cartdim_get(comm, &realCount);
    EXPECT_EQ(count, realCount);
    MPI_Cart_get(comm, count, dim, period, coord);
    MPI_Topo_test(comm, &status);
    EXPECT_EQ(status, MPI_CART);
}

TEST(Parallel_Operations_MPI, Test_Periods) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm comm = HyperCube();
    int count = DimCount(size);
    int realCount, status;

    int* dim = new int[count];
    int* period = new int[count];
    int* coord = new int[count];
    MPI_Cartdim_get(comm, &realCount);
    EXPECT_EQ(count, realCount);
    MPI_Cart_get(comm, count, dim, period, coord);
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(period[i], 1);
    }
    MPI_Topo_test(comm, &status);
    EXPECT_EQ(status, MPI_CART);
}

TEST(Parallel_Operations_MPI, Test_Dimention) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm comm = HyperCube();
    int count = DimCount(size);
    int realCount, status;

    int* dim = new int[count];
    int* period = new int[count];
    int* coord = new int[count];

    MPI_Cartdim_get(comm, &realCount);
    EXPECT_EQ(count, realCount);
    MPI_Cart_get(comm, count, dim, period, coord);
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(dim[i], 2);
    }
    MPI_Topo_test(comm, &status);
    EXPECT_EQ(status, MPI_CART);
}

TEST(Parallel_Operations_MPI, Trans_Data) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm comm = HyperCube();
    int count = DimCount(size);
    int tmp = -1, a, b;

    for (int i = 0; i < count; i++) {
        MPI_Cart_shift(comm, i, 1, &a, &b);
        if ((a == MPI_PROC_NULL) || (b == MPI_PROC_NULL)) {
            tmp = -2;
        }
    }
    if (rank == 0) {
        tmp = -2;
    }
    for (int i = 0; i < size; i++)
        MPI_Bcast(&tmp, 1, MPI_INT, 0, comm);
    ASSERT_EQ(tmp, -2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
