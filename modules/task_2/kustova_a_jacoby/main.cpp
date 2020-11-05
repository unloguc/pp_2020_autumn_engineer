// Copyright 2020 Kustova Anastasiya
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "./Jacoby.h"
#define MAX_ITERATIONS 100000
// using namespace std;

TEST(Jacoby_Method, Test_solve_1_system) {
    int size, rank, n;
    n = 2;
    double *Input_A, *Input_B, *X_New;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double sum = 0;
    if (rank == 0) {
        Input_A = new double[n * n];
        Input_B = new double[n];
        Input_A[0] = 4; Input_A[1] = 2; Input_A[2] = 1; Input_A[3] = 3;
        Input_B[0] = 1; Input_B[1] = -1;
    }
    X_New = new double[n];
    X_New = Parallel_Jacoby(Input_A, Input_B, n);
// Output vector
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                // cout << X_New[irow] << endl;
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum - Input_B[i]), 0.1);
        }
    }
}

TEST(Jacoby_Method, Test_solve_2_system) {
    int size, rank, n;
    n = 3;
    double *Input_A, *Input_B, *X_New;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double sum = 0;
    if (rank == 0) {
        Input_A = new double[n * n];
        Input_B = new double[n];
        Input_A[0] = 10; Input_A[1] = 1; Input_A[2] = -1; Input_A[3] = 1;
        Input_A[4] = 10; Input_A[5] = -1; Input_A[6] = -1; Input_A[7] = 1; Input_A[8] = 10;
        Input_B[0] = 11; Input_B[1] = 10; Input_B[2] = 10;
    }
    X_New = new double[n];
    X_New = Parallel_Jacoby(Input_A, Input_B, n);
// Output vector
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                // cout << X_New[irow] << endl;
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum - Input_B[i]), 0.1);
        }
    }
}


TEST(Jacoby_Method, Test_solve_2seq_system) {
    int rank, n;
    n = 3;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        double *Input_A, *Input_B, *X_New;
        double sum = 0;
        Input_A = new double[n * n];
        Input_B = new double[n];
        Input_A[0] = 10; Input_A[1] = 1; Input_A[2] = -1; Input_A[3] = 1;
        Input_A[4] = 10; Input_A[5] = -1; Input_A[6] = -1; Input_A[7] = 1; Input_A[8] = 10;
        Input_B[0] = 11; Input_B[1] = 10; Input_B[2] = 10;
        X_New = new double[n];
        X_New = Sequential_Jacoby(Input_A, Input_B, n);
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                // cout << X_New[irow] << endl;
                sum += X_New[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum - Input_B[i]), 0.1);
        }
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
