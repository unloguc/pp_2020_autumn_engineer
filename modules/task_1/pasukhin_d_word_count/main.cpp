//  Copyright 2020 Pasukhin Dmitry

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "./word_count.h"

TEST(Parallel_Operations_MPI, Test_Rand_String_Sum) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string global_string;
  const int count_size_string = 100;

  if (rank == 0) {
    global_string = getRandomString(count_size_string);
  }

  int global_sum = getParallelOperations(global_string, count_size_string);

  if (rank == 0) {
    int aFirstLetter = 0;
    int aLastLetter = 0;
    int reference_sum = getSequentialOperations(global_string, count_size_string , &aFirstLetter, &aLastLetter);
    ASSERT_EQ(reference_sum, global_sum);
  }
}

TEST(Parallel_Operations_MPI, Test_Empty_String_Sum) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string global_string = "";
  const int count_size_string = 0;

  int global_sum = getParallelOperations(global_string, count_size_string);

  if (rank == 0) {
    ASSERT_EQ(0, global_sum);
  }
}

TEST(Parallel_Operations_MPI, Test_Const_String_Sum) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  const int count_size_string = 27;
  std::string global_string("Global string  g l o b a l");

  int global_sum = getParallelOperations(global_string, count_size_string);

  if (rank == 0) {
    ASSERT_EQ(8, global_sum);
  }
}

TEST(Parallel_Operations_MPI, Test_One_Word_String_Sum) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string global_string("Global");
  const int count_size_string = 7;

  int global_sum = getParallelOperations(global_string, count_size_string);

  if (rank == 0) {
    ASSERT_EQ(1, global_sum);
  }
}

TEST(Parallel_Operations_MPI, Test_One_Letter_Words_String_Sum) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string global_string("  G l o b a l   s t r i n g   c h e c k i n g  ");
  const int count_size_string = 48;

  int global_sum = getParallelOperations(global_string, count_size_string);

  if (rank == 0) {
    ASSERT_EQ(20, global_sum);
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
