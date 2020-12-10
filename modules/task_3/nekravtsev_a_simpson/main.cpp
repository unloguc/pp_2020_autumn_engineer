// Copyright 2020 Alexey Nekravtsev
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "../../modules/task_3/nekravtsev_a_simpson/simpson.h"

TEST(Parallel_Operations_MPI, Test_Sequantial_Result) {
  int procRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  if (procRank == 0) {
    EXPECT_DOUBLE_EQ(Sequential_SimpsonForDouble(func, 0, 3, -2, 4, 10, 8), 10.8) << "Wrong result";
  }
}

TEST(Parallel_Operations_MPI, Test_Parallel_Result) {
  int procRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  double result = Parallel_SimpsonForDouble(func, 1, 14, 2, 5, 10, 10);

  if (procRank == 0) {
    EXPECT_DOUBLE_EQ(result, 409.5) << "Wrong result";
  }
}

TEST(Parallel_Operations_MPI, Test_Exception) {
  int procRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  if (procRank == 0) {
    EXPECT_ANY_THROW(Sequential_SimpsonForDouble(func, 0, 3, 5, 4, 10, 8));
  }
}

TEST(Parallel_Operations_MPI, Test_Parallel_Result_equals_Sequential_Result_small) {
  int procRank;
  double t1, t2, t3, t4;
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  t1 = MPI_Wtime();
  double parRes = Parallel_SimpsonForDouble(func, 0, 3, -2, 4, 30, 20);
  t2 = MPI_Wtime();
  t3 = MPI_Wtime();
  double seqRes = Sequential_SimpsonForDouble(func, 0, 3, -2, 4, 30, 20);
  t4 = MPI_Wtime();
  if (procRank == 0) {
    std::cout << "parallel: " << (t2 - t1) << ", seqential: " << (t4 - t3) << std::endl;
    EXPECT_DOUBLE_EQ(seqRes, parRes) << "Wrong result";
  }
}

TEST(Parallel_Operations_MPI, Test_Parallel_Result_equals_Sequential_Result_big) {
  int procRank;
  double t1, t2, t3, t4;
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  t1 = MPI_Wtime();
  double parRes = Parallel_SimpsonForDouble(func, -10, 31, 1, 15, 100, 80);
  t2 = MPI_Wtime();
  t3 = MPI_Wtime();
  double seqRes = Sequential_SimpsonForDouble(func, -10, 31, 1, 15, 100, 80);
  t4 = MPI_Wtime();
  if (procRank == 0) {
    std::cout << "parallel: " << (t2 - t1) << ", seqential: " << (t4 - t3) << std::endl;
    EXPECT_DOUBLE_EQ(seqRes, parRes) << "Wrong result";
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
