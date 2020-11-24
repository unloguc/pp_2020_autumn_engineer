// Copyright 2020 MERYEM EL FAHLI
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <mpi.h>
#include <algorithm>
#include <functional>
#include <vector>
#include "./hor_rib.h"

TEST(Parallel_Operations_MPI, 6x6) {
    int Num, Rank;
    MPI_Comm_size(MPI_COMM_WORLD, &Num);
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    int l = 6;
    int c = 6;
    std::vector<double> m;
    std::vector<double> vec;
    if (Rank == 0) {
        m = gen_mat(l, c);
        vec = gen_vec(l);
    }
    std::vector<double> v1 = par(m, vec, l, c);
    if (Rank == 0) {
        std::vector<double> v2 = seq(m, vec, l, c);
        ASSERT_EQ(v1, v2);
    }
}

TEST(Parallel_Operations_MPI, 1x7) {
    int Num, Rank;
    MPI_Comm_size(MPI_COMM_WORLD, &Num);
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    int l = 1;
    int c = 7;
    std::vector<double> m;
    std::vector<double> vec;
    if (Rank == 0) {
        m = gen_mat(l, c);
        vec = gen_vec(l);
    }
    std::vector<double> v1 = par(m, vec, l, c);
    if (Rank == 0) {
        std::vector<double> v2 = seq(m, vec, l, c);
        ASSERT_EQ(v1, v2);
    }
}

TEST(Parallel_Operations_MPI, 8x1) {
    int Num, Rank;
    MPI_Comm_size(MPI_COMM_WORLD, &Num);
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    int l = 8;
    int c = 1;
    std::vector<double> m;
    std::vector<double> vec;
    if (Rank == 0) {
        m = gen_mat(l, c);
        vec = gen_vec(l);
    }
    std::vector<double> v1 = par(m, vec, l, c);
    if (Rank == 0) {
        std::vector<double> v2 = seq(m, vec, l, c);
        ASSERT_EQ(v1, v2);
    }
}

TEST(Parallel_Operations_MPI, 3x3) {
    int Num, Rank;
    MPI_Comm_size(MPI_COMM_WORLD, &Num);
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    int l = 3;
    int c = 3;
    std::vector<double> m;
    std::vector<double> vec;
    if (Rank == 0) {
        m = gen_mat(l, c);
        vec = gen_vec(l);
    }
    std::vector<double> v1 = par(m, vec, l, c);
    if (Rank == 0) {
        std::vector<double> v2 = seq(m, vec, l, c);
        ASSERT_EQ(v1, v2);
    }
}

TEST(Parallel_Operations_MPI, 16x16) {
    int Num, Rank;
    MPI_Comm_size(MPI_COMM_WORLD, &Num);
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    int l = 16;
    int c = 16;
    std::vector<double> m;
    std::vector<double> vec;
    if (Rank == 0) {
        m = gen_mat(l, c);
        vec = gen_vec(l);
    }
    std::vector<double> v1 = par(m, vec, l, c);
    if (Rank == 0) {
        std::vector<double> v2 = seq(m, vec, l, c);
        ASSERT_EQ(v1, v2);
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
