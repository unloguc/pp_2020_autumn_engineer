// Copyright 2020 Ognev Denis

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "../../modules/task_3/ognev_d_quick_sort/sort.h"

TEST(Parallel_Test, Throws_When_Size_Is_Less_Than_1) {
  int size = 0;
  int* vec = getRandomVector(1);
  ASSERT_ANY_THROW(quickSortWithMergeParallel(vec, size));
}

TEST(Parallel_Test, Works_On_Size_1) {
  int size = 1;
  int* vec = getRandomVector(size);
  ASSERT_NO_THROW(quickSortWithMergeParallel(vec, size));
}

TEST(Parallel_Test, Works_Right_On_Size_2) {
  int size = 2;
  int* vec = getRandomVector(size);
  int* res;
  ASSERT_NO_THROW(res = quickSortWithMergeParallel(vec, size));
  for (int i = 0; i < size - 1; i++) {
      ASSERT_LE(res[i], res[i + 1]);
  }
}

TEST(Parallel_Test, Works_Right_On_Size_5) {
  int size = 5;
  int* vec = getRandomVector(size);
  int* res;
  ASSERT_NO_THROW(res = quickSortWithMergeParallel(vec, size));
  for (int i = 0; i < size - 1; i++) {
      ASSERT_LE(res[i], res[i + 1]);
  }
}

TEST(Parallel_Test, Time_Test) {
  int rank, proc_num;
  double start_time, end_time;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
  const int size = 3;
  int n = proc_num;
  int* array = getRandomVector(size);

  start_time = MPI_Wtime();
  quickSortWithMergeParallel(array, size);
  end_time = MPI_Wtime();
  if (rank == 0) {
    std::cout << "Parallel time = " << end_time - start_time << "\n";
  }

  start_time = MPI_Wtime();
  quickSortWithMergeSequential(array, size, n);
  end_time = MPI_Wtime();
  if (rank == 0) {
    std::cout << "Sequential time = " << end_time - start_time << "\n";
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
