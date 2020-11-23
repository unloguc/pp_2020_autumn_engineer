// Copyright 2020 Kustova Anastasiya
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "./Jacoby.h"
// using namespace std;

TEST(Jacoby_Method, Test_solve_1_system) {
    int rank, n, size;
    n = 8;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 0.001;
    double sum = 0;
    std::vector<double>Input_A(n*n);
    std::vector<double>Input_B(n);
    if (rank == 0) {
        //     (61 2  3  4  6  8  9  2 )       (95)
        //     (2  84 6  4  3  2  8  7 )       (116)
        //     (3  6  68 2  4  3  9  1 )       (96)
        // A = (4  4  2  59 6  4  3  8 )   B = (90)
        //     (6  3  4  6  93 8  3  1 )       (124)
        //     (8  2  3  4  8  98 3  1 )       (127)
        //     (9  8  9  3  3  3  85 7 )       (127)
        //     (2  7  1  8  1  1  7  98)       (125)
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
    }
    std::vector<double> X_New(n);
    if (size <= 8) {
            X_New = Parallel_Jacoby(Input_A, Input_B, n, eps);
    } else {
        X_New = Sequential_Jacoby(Input_A, Input_B, n, eps);
    }
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum - Input_B[i]), 0.1);
        }
    }
}

TEST(Jacoby_Method, Test_solve_2_system) {
    int rank, n, size;
    n = 3;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 0.001;
    double sum = 0;
    std::vector<double>Input_A(n*n);
    std::vector<double>Input_B(n);
    if (rank == 0) {
        //     ( 10  1 -1 )        (11)
        // A = ( 1  10 -1 )    B = (10)
        //     (-1  1  10 )        (10)
        Input_A = {10, 1, -1, 1, 10, -1, -1, 1, 10};
        Input_B = {11, 10, 10};
    }
    std::vector<double> X_New(n);
    if (size <= 3) {
        X_New = Parallel_Jacoby(Input_A, Input_B, n, eps);
    } else {
        X_New = Sequential_Jacoby(Input_A, Input_B, n, eps);
    }
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
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
        double eps = 0.001;
        double sum;
        std::vector<double> Input_A(n*n);
        std::vector<double> Input_B(n);
        //     ( 10  1 -1 )        (11)
        // A = ( 1  10 -1 )    B = (10)
        //     (-1  1  10 )        (10)
        Input_A = {10, 1, -1, 1, 10, -1, -1, 1, 10};
        Input_B = {11, 10, 10};
        std::vector<double> X_New(n);
        X_New = Sequential_Jacoby(Input_A, Input_B, n, eps);
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                sum += X_New[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum - Input_B[i]), 0.1);
        }
    }
}

TEST(Jacoby_Method, Test_solve_3_system) {
    int rank, n, size;
    n = 2;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 0.001;
    double sum = 0;
    std::vector<double>Input_A(n*n);
    std::vector<double>Input_B(n);
    if (rank == 0) {
        // A = ( 4  2 )    B = (1)
        //     ( 1  3 )        (-1)
        Input_A[0] = 4; Input_A[1] = 2; Input_A[2] = 1; Input_A[3] = 3;
        Input_B[0] = 1; Input_B[1] = -1;
    }
    std::vector<double> X_New(n);
    if (size <= 2) {
        X_New = Parallel_Jacoby(Input_A, Input_B, n, eps);
    } else {
        X_New = Sequential_Jacoby(Input_A, Input_B, n, eps);
    }
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum - Input_B[i]), 0.1);
        }
    }
}

TEST(Jacoby_Method, Test_solve_4_system) {
    int rank, n, size;
    n = 3;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 0.001;
    double sum = 0;
    std::vector<double>Input_A(n*n);
    std::vector<double>Input_B(n);
    if (rank == 0) {
        //     ( 115 -20 -75 )        ( 20 )
        // A = ( 15  -50  -5 )    B = (-40 )
        //     ( 6    2   20 )        ( 28 )
        Input_A[0] = 115; Input_A[1] = -20; Input_A[2] = -75; Input_A[3] = 15;
        Input_A[4] = -50; Input_A[5] = -5; Input_A[6] = 6; Input_A[7] = 2; Input_A[8] = 20;
        Input_B[0] = 20; Input_B[1] = -40; Input_B[2] = 28;
    }
    std::vector<double> X_New(n);
    if (size <= 3) {
        X_New = Parallel_Jacoby(Input_A, Input_B, n, eps);
    } else {
        X_New = Sequential_Jacoby(Input_A, Input_B, n, eps);
    }
    if (rank == 0) {
        for (int i = 0; i < n; i ++) {
            sum = 0;
            for (int irow = 0; irow < n; irow ++) {
                sum+=X_New[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum - Input_B[i]), 0.1);
        }
    }
}

TEST(Jacoby_Method, Test_solve_random_matrix) {
    int rank, n, size;
    n = 1000;
    double start_time1, end_time1, start_time2, end_time2;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 0.0000001;
    double sum1 = 0, sum2 = 0;
    std::vector<double>Input_A(n*n);
    std::vector<double>Input_B(n);
    if (rank == 0) {
        std::vector<double>matrix(n*n + n);
        matrix = Gen_Matrix(n);
        for (int i = 0; i < n * n; i++) {
            Input_A[i] = matrix[i];
        }
        for (int i = 0; i < n; i++) {
            Input_B[i] = matrix[n * n + i];
        }
    }
    std::vector<double> X_New1(n);
    std::vector<double> X_New2(n);
    if (size <= n) {
        start_time1 = MPI_Wtime();
        X_New1 = Parallel_Jacoby(Input_A, Input_B, n, eps);
        end_time1 = MPI_Wtime();
        start_time2 = MPI_Wtime();
        X_New2 = Sequential_Jacoby(Input_A, Input_B, n, eps);
        end_time2 = MPI_Wtime();
    } else {
        X_New1 = X_New2 = Sequential_Jacoby(Input_A, Input_B, n, eps);
    }
    if (rank == 0) {
        std::cout << "Parallel time = " << end_time1 - start_time1 << std::endl;
        std::cout << "Sequential time = " << end_time2 - start_time2 << std::endl;
        for (int i = 0; i < n; i ++) {
            sum1 = 0;
            sum2 = 0;
            for (int irow = 0; irow < n; irow ++) {
                sum1+=X_New1[irow] * Input_A[i * n + irow];
                sum2+=X_New2[irow] * Input_A[i * n + irow];
            }
            ASSERT_LE(my_abs(sum1 - Input_B[i]), 0.1);
            ASSERT_LE(my_abs(sum2 - Input_B[i]), 0.1);
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
