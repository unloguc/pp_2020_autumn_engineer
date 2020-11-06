// Copyright 2020 Kustova Anastasiya
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "./Jacoby.h"
#define MAX_ITERATIONS 100000
// using namespace std;
double my_abs(double x);
TEST(Jacoby_Method, Test_solve_1_system) {
    int size, rank, n, amountRowBloc, GlobalRowNo;
    n = 8;
    double *Input_A, *Input_B, *ARecv, *BRecv, *Bloc_XX, *X_New, *X_Old, *Bloc_X;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 0.001;
    double sum = 0;
    double norm;
    if (rank == 0) {
        Input_A = new double[n * n];
        Input_B = new double[n];
      // Input_A[0] = 4; Input_A[1] = 2; Input_A[2] = 1; Input_A[3] = 3;
      //  Input_B[0] = 1; Input_B[1] = -1;
      Input_A[0] = 61; Input_A[1] = 2; Input_A[2] = 3; Input_A[3] = 4;
Input_A[4] = 6; Input_A[5] = 8; Input_A[6] = 9; Input_A[7] = 2; Input_A[8] = 2;
Input_A[9] = 84; Input_A[10] = 6; Input_A[11] = 4; Input_A[12] = 3;
Input_A[13] = 2; Input_A[14] = 8; Input_A[15] = 7; Input_A[16] = 3; Input_A[17] = 6;
Input_A[18] = 68; Input_A[19] = 2; Input_A[20] = 4; Input_A[21] = 3;
Input_A[22] = 9; Input_A[23] = 1; Input_A[24] = 4; Input_A[25] = 4; Input_A[26] = 2;
Input_A[27] = 59; Input_A[28] = 6; Input_A[29] = 4; Input_A[30] = 3;
Input_A[31] = 8; Input_A[32] = 6; Input_A[33] = 3; Input_A[34] = 4; Input_A[35] = 6;
Input_A[36] = 93; Input_A[36] = 8; Input_A[38] = 3; Input_A[39] = 1;
Input_A[40] = 8; Input_A[41] = 2; Input_A[42] = 3; Input_A[43] = 4;
Input_A[44] = 8; Input_A[45] = 98; Input_A[46] = 3; Input_A[47] = 1;
Input_A[48] = 9; Input_A[49] = 8; Input_A[50] = 9; Input_A[51] = 3; Input_A[52] = 3;
Input_A[53] = 3; Input_A[54] = 85; Input_A[55] = 7; Input_A[56] = 2;
Input_A[57] = 7; Input_A[58] = 1; Input_A[59] = 8; Input_A[60] = 1; Input_A[61] = 1;
Input_A[62] = 7; Input_A[63] = 98;

Input_B[0] = 95; Input_B[1] = 116; Input_B[2] = 96; Input_B[3] = 90;
Input_B[4] = 124; Input_B[5] = 127; Input_B[6] = 127; Input_B[7] = 125;
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
        norm = my_abs(X_New[0] - Bloc_X[0]);
            for (int h = 0; h < n; h++) {
                if (my_abs(X_New[h] - Bloc_X[h]) > norm) {
                    norm = my_abs(X_New[h] - Bloc_X[h]);
                }
            }
        MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);

        if (rank == 0) {
            Bloc_XX = Iteration_for_0_rank(n, X_Old, Input_B, Bloc_XX, Input_A, GlobalRowNo,  amountRowBloc, size);
            for (int h = 0; h < n; h++) {
                if (my_abs(X_New[h] - Bloc_XX[h]) > norm) {
                    norm = my_abs(X_New[h] - Bloc_XX[h]);
                }
            }
        }

        for (int irow = amountRowBloc * size; irow < n; irow ++) {
            MPI_Allgather(&Bloc_XX[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
        }

        Iteration++;
    }while((Iteration < MAX_ITERATIONS) && (norm >= eps));

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

double my_abs(double x) {
    if (x > 0) {
        return x;
    } else {
        return -x;
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
        double eps = 0.001, norm, * TempX = new double[n];
        for (int k = 0; k < n; k++) {
            TempX[k] = X_New[k];
        }
        int cnt = 0;
        do {
            for (int i = 0; i < n; i++) {
                TempX[i] = Input_B[i];
                for (int g = 0; g < n; g++) {
                    if (i != g) {
                        TempX[i] -= Input_A[i * n + g] * X_New[g];
                    }
                }
                TempX[i] /= Input_A[i * n + i];
            }
            norm = my_abs(X_New[0] - TempX[0]);
            for (int h = 0; h < n; h++) {
                if (my_abs(X_New[h] - TempX[h]) > norm) {
                    norm = my_abs(X_New[h] - TempX[h]);
                }
                X_New[h] = TempX[h];
            }
            cnt++;
        } while (norm > eps);
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                // cout << X_New[irow] << endl;
                sum += X_New[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum - Input_B[i]), 0.1);
        }
        delete[] TempX;
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
