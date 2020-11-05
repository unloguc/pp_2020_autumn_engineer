// Copyright 2020 Kustova Anastasiya
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "./Jacoby.h"
#define MAX_ITERATIONS 10
// using namespace std;
/*
TEST(Jacoby_Method, Test_solve_1_system) {
    int size, rank, n, amountRowBloc, GlobalRowNo;
    n = 2;
    double *Input_A, *Input_B, *ARecv, *BRecv, *Bloc_XX, *X_New, *X_Old, *Bloc_X;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 0.001;
    double sum = 0;
    if (rank == 0) {
        Input_A = new double[n * n];
        Input_B = new double[n];
        Input_A[0] = 4; Input_A[1] = 2; Input_A[2] = 1; Input_A[3] = 3;
        Input_B[0] = 1; Input_B[1] = -1;
// Input_A[0] = 115; Input_A[1] = -20; Input_A[2] = -75; Input_A[3] = 15;
// Input_A[4] = -50; Input_A[5] = -5; Input_A[6] = 6; Input_A[7] = 2; Input_A[8] = 20;
// Input_B[0] = 20; Input_B[1] = -40; Input_B[2] = 28;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    amountRowBloc = n / size;    // how much to each proc
    ARecv = new double[amountRowBloc * n];
    BRecv = new double[amountRowBloc];

    MPI_Scatter(Input_A, amountRowBloc * n, MPI_DOUBLE, ARecv, amountRowBloc * n, MPI_DOUBLE, 0,     MPI_COMM_WORLD);
    MPI_Scatter(Input_B, amountRowBloc, MPI_DOUBLE, BRecv, amountRowBloc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    Bloc_X = new double[n];
    X_New = new double[n];
    X_Old = new double[n];

    for (int irow=0; irow < amountRowBloc; irow ++) {
        Bloc_X[irow] = BRecv[irow];
    }

    MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);
    int Iteration = 0;

    for (int irow = amountRowBloc * size; irow < n; irow++) {
        MPI_Allgather(&Input_B[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
    }

    Bloc_XX = new double[n];
    do {
        Bloc_X = Iterations(n, X_Old, X_New, Bloc_X, BRecv, ARecv, GlobalRowNo, amountRowBloc, rank);
        MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);

        if (rank == 0) {
            Bloc_XX = Iteration_for_0_rank(n, X_Old, Input_B, Bloc_XX, Input_A, GlobalRowNo,  amountRowBloc, size);
        }

        for (int irow = amountRowBloc * size; irow < n; irow ++) {
            MPI_Allgather(&Bloc_XX[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        Iteration++;
    }while((Iteration < MAX_ITERATIONS) && (Distance(X_Old, X_New, n) >= eps));

// Output vector
    double s;
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                // cout << X_New[irow] << endl;
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            if (sum - Input_B[i] > 0) {
                s = sum - Input_B[i];
            } else {
                s = -(sum - Input_B[i]);
            }
            ASSERT_LE(s, 0.1);
        }
    }
}

TEST(Jacoby_Method, Test_solve_2_system) {
    int size, rank, n, amountRowBloc, GlobalRowNo;
    n = 3;
    double *Input_A, *Input_B, *ARecv, *BRecv, *Bloc_XX, *X_New, *X_Old, *Bloc_X;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 0.001;
    double sum = 0;
    if (rank == 0) {
        Input_A = new double[n * n];
        Input_B = new double[n];
        Input_A[0] = 10; Input_A[1] = 1; Input_A[2] = -1; Input_A[3] = 1;
        Input_A[4] = 10; Input_A[5] = -1; Input_A[6] = -1; Input_A[7] = 1; Input_A[8] = 10;
        Input_B[0] = 11; Input_B[1] = 10; Input_B[2] = 10;
    }
MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    amountRowBloc = n / size;    // how much to each proc
    ARecv = new double[amountRowBloc * n];
    BRecv = new double[amountRowBloc];

    MPI_Scatter(Input_A, amountRowBloc * n, MPI_DOUBLE, ARecv, amountRowBloc * n, MPI_DOUBLE, 0,     MPI_COMM_WORLD);
    MPI_Scatter(Input_B, amountRowBloc, MPI_DOUBLE, BRecv, amountRowBloc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    Bloc_X = new double[n];
    X_New = new double[n];
    X_Old = new double[n];

    for (int irow=0; irow < amountRowBloc; irow ++) {
        Bloc_X[irow] = BRecv[irow];
    }

    MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);
    int Iteration = 0;

    for (int irow = amountRowBloc * size; irow < n; irow++) {
        MPI_Allgather(&Input_B[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
    }

    Bloc_XX = new double[n];
    do {
        Bloc_X = Iterations(n, X_Old, X_New, Bloc_X, BRecv, ARecv, GlobalRowNo, amountRowBloc, rank);
        MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);

        if (rank == 0) {
            Bloc_XX = Iteration_for_0_rank(n, X_Old, Input_B, Bloc_XX, Input_A, GlobalRowNo,  amountRowBloc, size);
        }

        for (int irow = amountRowBloc * size; irow < n; irow ++) {
            MPI_Allgather(&Bloc_XX[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        Iteration++;
    }while((Iteration < MAX_ITERATIONS) && (Distance(X_Old, X_New, n) >= eps));

// Output vector
    double s;
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                // cout << X_New[irow] << endl;
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            if (sum - Input_B[i] > 0) {
                s = sum - Input_B[i];
            } else {
                s = -(sum - Input_B[i]);
            }
            ASSERT_LE(s, 0.1);
        }
    }
}

TEST(Jacoby_Method, Test_solve_2_system_not_parallel_version) {
    int size, rank, n;
    n = 3;
    double Input_A[n * n], Input_B[n], *X_New;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double sum = 0;
    if (rank == 0) {
        Input_A[0] = 10; Input_A[1] = 1; Input_A[2] = -1; Input_A[3] = 1;
        Input_A[4] = 10; Input_A[5] = -1; Input_A[6] = -1; Input_A[7] = 1; Input_A[8] = 10;
        Input_B[0] = 11; Input_B[1] = 10; Input_B[2] = 10;
        X_New = new double[n];
        X_New = Sequential_Jacoby(Input_A, Input_B, n);
// Output vector
        double s;
        if (rank == 0) {
            for (int i = 0; i < n; i ++) {
                sum = 0;
                for (int irow = 0; irow < n; irow ++) {
                    // cout << X_New[irow] << endl;
                    sum+=X_New[irow] * Input_A[i * n + irow];
                }
                if (sum - Input_B[i] > 0) {
                    s = sum - Input_B[i];
                } else {
                    s = -(sum - Input_B[i]);
                }
                ASSERT_LE(s, 0.1);
            }
        }
    delete [] X_New;
    }
}
*/
TEST(Jacoby_Method, Test_solve_1_system) {
    int size, rank, n;
    n = 2;
    double *X_New, Input_A[n * n], Input_B[n];
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double sum = 0;
    if (rank == 0) {
        Input_A[0] = 4; Input_A[1] = 2; Input_A[2] = 1; Input_A[3] = 3;
        Input_B[0] = 1; Input_B[1] = -1;
// Input_A[0] = 115; Input_A[1] = -20; Input_A[2] = -75; Input_A[3] = 15;
// Input_A[4] = -50; Input_A[5] = -5; Input_A[6] = 6; Input_A[7] = 2; Input_A[8] = 20;
// Input_B[0] = 20; Input_B[1] = -40; Input_B[2] = 28;
    }
    X_New = new double[n];
    X_New = Parallel_Jacoby(Input_A, Input_B, n);
// Output vector
    double s;
    if (rank == 0) {
          for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                // cout << X_New[irow] << endl;
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            if (sum - Input_B[i] > 0) {
                s = sum - Input_B[i];
            } else {
                s = -(sum - Input_B[i]);
            }
            ASSERT_LE(s, 0.1);
        }
    delete [] X_New;
    }
}

TEST(Jacoby_Method, Test_solve_2_system) {
    int size, rank, n;
    n = 3;
    double *X_New, Input_A[n * n], Input_B[n];
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double sum = 0;
    if (rank == 0) {
        Input_A[0] = 10; Input_A[1] = 1; Input_A[2] = -1; Input_A[3] = 1;
        Input_A[4] = 10; Input_A[5] = -1; Input_A[6] = -1; Input_A[7] = 1; Input_A[8] = 10;
        Input_B[0] = 11; Input_B[1] = 10; Input_B[2] = 10;
    }
X_New = new double[n];
X_New = Parallel_Jacoby(Input_A, Input_B, n);
// Output vector
    double s;
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                // cout << X_New[irow] << endl;
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            if (sum - Input_B[i] > 0) {
                s = sum - Input_B[i];
            } else {
                s = -(sum - Input_B[i]);
            }
            ASSERT_LE(s, 0.1);
        }
    delete [] X_New;
    }
}


TEST(Jacoby_Method, Test_solve_2_system_not_parallel_version) {
    int size, rank, n;
    n = 3;
    double Input_A[n * n], Input_B[n], *X_New;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double sum = 0;
    if (rank == 0) {
        Input_A[0] = 10; Input_A[1] = 1; Input_A[2] = -1; Input_A[3] = 1;
        Input_A[4] = 10; Input_A[5] = -1; Input_A[6] = -1; Input_A[7] = 1; Input_A[8] = 10;
        Input_B[0] = 11; Input_B[1] = 10; Input_B[2] = 10;
        X_New = new double[n];
        X_New = Sequential_Jacoby(Input_A, Input_B, n);
// Output vector
        double s;
        if (rank == 0) {
            for (int i = 0; i < n; i ++) {
                sum = 0;
                for (int irow = 0; irow < n; irow ++) {
                    // cout << X_New[irow] << endl;
                    sum+=X_New[irow] * Input_A[i * n + irow];
                }
                if (sum - Input_B[i] > 0) {
                    s = sum - Input_B[i];
                } else {
                    s = -(sum - Input_B[i]);
                }
                ASSERT_LE(s, 0.1);
            }
        }
    delete [] X_New;
    }
}

TEST(Jacoby_Method, Test_solve_3_system) {
    int size, rank, n;
    n = 3;
    double *X_New, Input_A[n * n], Input_B[n];
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double sum = 0;
    if (rank == 0) {
        Input_A[0] = 8; Input_A[1] = 1; Input_A[2] = -4; Input_A[3] = 2;
        Input_A[4] = -6; Input_A[5] = 1; Input_A[6] = -1; Input_A[7] = 1; Input_A[8] = 4;
        Input_B[0] = 6; Input_B[1] = -9; Input_B[2] = 5;
    }
X_New = new double[n];
X_New = Parallel_Jacoby(Input_A, Input_B, n);
// Output vector
    double s;
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
               //  cout << X_New[irow] << endl;
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            if (sum - Input_B[i] > 0) {
                s = sum - Input_B[i];
            } else {
                s = -(sum - Input_B[i]);
            }
            ASSERT_LE(s, 0.1);
        }
    delete [] X_New;
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
