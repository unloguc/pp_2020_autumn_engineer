// Copyright 2020 Chesnokov Artyom
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./matrix_column_sum.h"

TEST(Task_1, Test_Random_Matrix_No_Throw) {
    EXPECT_NO_THROW(Matrix mat = getRandomMatrix(10, 10));
}

TEST(Task_1, Test_Sequential_On_Predefined_Matrix) {
  int data[] = { 4, 2, 1, 8,  2, 3, 6, -2,  3, 3, 5, 1 };
  std::vector<int> res;
  std::vector<int> seq_res;

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    Matrix matrix(3, 4, data);
    res = std::vector<int>({15, 9, 12});
    seq_res = getSequentialColumnSum(matrix);

    EXPECT_EQ(seq_res, res);
  }
}

TEST(Task_1, Test_Parallel_On_Predefined_Matrix) {
  int data[] = { 4, 2, 1, 8,  2, 3, 6, -2,  3, 3, 5, 1 };
  std::vector<int> res;
  std::vector<int> par_res;

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  Matrix matrix(3, 4, data);
  res = std::vector<int>({ 15, 9, 12 });

  par_res = getParallelColumnSum(matrix);
  if (rank == 0) {
    EXPECT_EQ(par_res, res);
  }
}

TEST(Task_1, Test_Sequential_And_Parallel_Sums_Are_The_Same_123x321) {
  Matrix mat = getRandomMatrix(123, 321);
  auto seq_res = getSequentialColumnSum(mat);
  auto par_res = getParallelColumnSum(mat);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    EXPECT_EQ(seq_res, par_res);
  }
}

TEST(Task_1, Test_Sequential_And_Parallel_Sums_Are_The_Same_1x10) {
  Matrix mat = getRandomMatrix(1, 10);
  auto seq_res = getSequentialColumnSum(mat);
  auto par_res = getParallelColumnSum(mat);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    EXPECT_EQ(seq_res, par_res);
  }
}

TEST(Task_1, Test_Sequential_And_Parallel_Sums_Are_The_Same_9x1) {
  Matrix mat = getRandomMatrix(9, 1);
  auto seq_res = getSequentialColumnSum(mat);
  auto par_res = getParallelColumnSum(mat);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    EXPECT_EQ(seq_res, par_res);
  }
}

TEST(Task_1, Test_Sequential_And_Parallel_Sums_Are_The_Same_23x32) {
  Matrix mat = getRandomMatrix(23, 32);
  auto seq_res = getSequentialColumnSum(mat);
  auto par_res = getParallelColumnSum(mat);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    EXPECT_EQ(seq_res, par_res);
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
