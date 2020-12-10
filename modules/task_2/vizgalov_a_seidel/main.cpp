// Copyright 2020 Vizgalov Anton
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>

#include "./seidel.h"


const int size1 = 4;
double A1[size1][size1] = {
    {20, 2, 3, 7},
    {1, 12, -2, -5},
    {5, -3, 13, 0},
    {0, 0, -1, 15},
};
double b1[size1] = {5, 4, -3, 7};

const int size2 = 8;
double A2[size2][size2] = {
    {45, 1, 4, 7, 3, 7, 5, -6},
    {3, 72, 6, -9, 6, 3, 5, 7},
    {-9, 3, 88, 6, 8, -5, 3, 5},
    {1, 5, 8, 61, 5, 9, 7, 6},
    {3, -5, 7, 6, 79, 7, 9, 3},
    {2, 1, 4, 4, 7, 54, 4, 3},
    {8, 4, -6, 3, 5, 2, 48, 1},
    {9, 5, 6, 6, -1, 3, 3, 82}
};
double b2[size2] = {54, 58, 36, 85, 36, 26, 83, 63};


TEST(Seidel, Seidel_Empty) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double precision;
    seidelParallel(0, nullptr, nullptr, 0, nullptr, &precision);
}

TEST(Seidel, Seidel_System_Size_4_Iterations_3) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double** mat = new double*[size1];
    for (int i = 0; i < size1; ++i) {
        mat[i] = new double[size1];
    }
    double* vec = new double[size1];
    double* res = new double[size1];
    double precision;

    if (rank == 0) {
        for (int i = 0; i < size1; ++i) {
            for (int j = 0; j < size1; ++j) {
                mat[i][j] = A1[i][j];
            }
        }
        for (int i = 0; i < size1; ++i) {
            vec[i] = b1[i];
        }
    }

    seidelParallel(size1, mat, vec, 3, res, &precision);

    if (rank == 0) {
        EXPECT_NEAR(0.067514, res[0], 1e-6);
        EXPECT_NEAR(0.490919, res[1], 1e-6);
        EXPECT_NEAR(-0.143447, res[2], 1e-6);
        EXPECT_NEAR(0.457104, res[3], 1e-6);
        EXPECT_DOUBLE_EQ(0.1, precision);
    }

    for (int i = 0; i < size1; ++i) {
        delete[] mat[i];
    }
    delete[] mat;
    delete[] vec;
    delete[] res;
}

TEST(Seidel, Seidel_System_Size_4_Iterations_7) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double** mat = new double*[size1];
    for (int i = 0; i < size1; ++i) {
        mat[i] = new double[size1];
    }
    double* vec = new double[size1];
    double* res = new double[size1];
    double precision;

    if (rank == 0) {
        for (int i = 0; i < size1; ++i) {
            for (int j = 0; j < size1; ++j) {
                mat[i][j] = A1[i][j];
            }
        }
        for (int i = 0; i < size1; ++i) {
            vec[i] = b1[i];
        }
    }

    seidelParallel(size1, mat, vec, 7, res, &precision);

    if (rank == 0) {
        EXPECT_NEAR(0.061404, res[0], 1e-6);
        EXPECT_NEAR(0.495424, res[1], 1e-6);
        EXPECT_NEAR(-0.140057, res[2], 1e-6);
        EXPECT_NEAR(0.457329, res[3], 1e-6);
        EXPECT_DOUBLE_EQ(0.0001, precision);
    }

    for (int i = 0; i < size1; ++i) {
        delete[] mat[i];
    }
    delete[] mat;
    delete[] vec;
    delete[] res;
}

TEST(Seidel, Seidel_System_Size_8_Iterations_3) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double** mat = new double*[size2];
    for (int i = 0; i < size2; ++i) {
        mat[i] = new double[size2];
    }
    double* vec = new double[size2];
    double* res = new double[size2];
    double precision;

    if (rank == 0) {
        for (int i = 0; i < size2; ++i) {
            for (int j = 0; j < size2; ++j) {
                mat[i][j] = A2[i][j];
            }
        }
        for (int i = 0; i < size2; ++i) {
            vec[i] = b2[i];
        }
    }

    seidelParallel(size2, mat, vec, 3, res, &precision);

    if (rank == 0) {
        EXPECT_NEAR(0.858404, res[0], 1e-6);
        EXPECT_NEAR(0.700745, res[1], 1e-6);
        EXPECT_NEAR(0.321197, res[2], 1e-6);
        EXPECT_NEAR(1.020141, res[3], 1e-6);
        EXPECT_NEAR(0.157576, res[4], 1e-6);
        EXPECT_NEAR(0.180528, res[5], 1e-6);
        EXPECT_NEAR(1.470077, res[6], 1e-6);
        EXPECT_NEAR(0.474736, res[7], 1e-6);
        EXPECT_DOUBLE_EQ(0.1, precision);
    }

    for (int i = 0; i < size2; ++i) {
        delete[] mat[i];
    }
    delete[] mat;
    delete[] vec;
    delete[] res;
}

TEST(Seidel, Seidel_System_Size_8_Iterations_7) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double** mat = new double*[size2];
    for (int i = 0; i < size2; ++i) {
        mat[i] = new double[size2];
    }
    double* vec = new double[size2];
    double* res = new double[size2];
    double precision;

    if (rank == 0) {
        for (int i = 0; i < size2; ++i) {
            for (int j = 0; j < size2; ++j) {
                mat[i][j] = A2[i][j];
            }
        }
        for (int i = 0; i < size2; ++i) {
            vec[i] = b2[i];
        }
    }

    seidelParallel(size2, mat, vec, 7, res, &precision);

    if (rank == 0) {
        EXPECT_NEAR(0.857559, res[0], 1e-6);
        EXPECT_NEAR(0.702026, res[1], 1e-6);
        EXPECT_NEAR(0.321713, res[2], 1e-6);
        EXPECT_NEAR(1.024507, res[3], 1e-6);
        EXPECT_NEAR(0.159735, res[4], 1e-6);
        EXPECT_NEAR(0.181059, res[5], 1e-6);
        EXPECT_NEAR(1.469853, res[6], 1e-6);
        EXPECT_NEAR(0.474409, res[7], 1e-6);
        EXPECT_DOUBLE_EQ(0.0001, precision);
    }

    for (int i = 0; i < size2; ++i) {
        delete[] mat[i];
    }
    delete[] mat;
    delete[] vec;
    delete[] res;
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
