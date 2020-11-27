// Copyright 2020 Krasilnikov Alexey

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "../../../modules/task_2/krasilnikov_a_bubble_sort/bubble_sort.h"

void CustomTest(const size_t test_size);

TEST(Parallel_Operations_MPI, Test_single) {
  CustomTest(1);
}

TEST(Parallel_Operations_MPI, Test_small_size) {
  CustomTest(10);
}

TEST(Parallel_Operations_MPI, Test_middle_size) {
  CustomTest(100);
}

TEST(Parallel_Operations_MPI, Test_big_size) {
  CustomTest(1000);
}

void CustomTest(const size_t vector_size) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::vector<int> vector_for_sequential_sort;
  std::vector<int> vector_for_parallel_sort;
  if (rank == 0) {
    vector_for_sequential_sort = getRandomVector(vector_size);
    vector_for_parallel_sort = vector_for_sequential_sort;
  }
  sortedArrayParallel(vector_for_parallel_sort.data(), vector_size);
  if (rank == 0) {
    sortedArraySequential(vector_for_sequential_sort.data(), vector_size);
    ASSERT_EQ(vector_for_parallel_sort, vector_for_sequential_sort);
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
