// Copyright 2020 Ognev Denis

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "../../modules/task_2/ognev_d_matrix_vector_vertical_multiplication/multiplication.h"

TEST(Parallel_Multiplication_Test, Works_On_1x1_Matrix) {
  int rows = 1, columns = 1, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  ASSERT_NO_THROW(parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize));
}

TEST(Parallel_Multiplication_Test, Works_On_1x5_Matrix) {
  int rows = 1, columns = 5, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  ASSERT_NO_THROW(parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize));
}

TEST(Parallel_Multiplication_Test, Works_On_5x1_Matrix) {
  int rows = 5, columns = 1, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  ASSERT_NO_THROW(parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize));
}

TEST(Parallel_Multiplication_Test, Parallel_Equals_Sequential_On_1x1_Matrix) {
  int rows = 1, columns = 1, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  int* sequentialResult = sequentialTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  int* parallelResult = parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  for (int i = 0; i < rows; i++) {
    EXPECT_EQ(sequentialResult[i], parallelResult[i]);
  }
}

TEST(Parallel_Multiplication_Test, Parallel_Equals_Sequential_On_1x5_Matrix) {
  int rows = 1, columns = 5, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  int* sequentialResult = sequentialTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  int* parallelResult = parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  for (int i = 0; i < rows; i++) {
    EXPECT_EQ(sequentialResult[i], parallelResult[i]);
  }
}

TEST(Parallel_Multiplication_Test, Parallel_Equals_Sequential_On_5x1_Matrix) {
  int rows = 5, columns = 1, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  int* sequentialResult = sequentialTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  int* parallelResult = parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  for (int i = 0; i < rows; i++) {
    EXPECT_EQ(sequentialResult[i], parallelResult[i]);
  }
}

TEST(Parallel_Multiplication_Test, Parallel_Equals_Sequential_On_3x3_Matrix) {
  int rows = 3, columns = 3, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  int* sequentialResult = sequentialTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  int* parallelResult = parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  for (int i = 0; i < rows; i++) {
    EXPECT_EQ(sequentialResult[i], parallelResult[i]);
  }
}

TEST(Parallel_Multiplication_Test, Parallel_Equals_Sequential_On_3x4_Matrix) {
  int rows = 3, columns = 4, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  int* sequentialResult = sequentialTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  int* parallelResult = parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  for (int i = 0; i < rows; i++) {
    EXPECT_EQ(sequentialResult[i], parallelResult[i]);
  }
}

TEST(Parallel_Multiplication_Test, Parallel_Equals_Sequential_On_4x3_Matrix) {
  int rows = 4, columns = 3, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  int* sequentialResult = sequentialTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  int* parallelResult = parallelTransposedMultiplication(matrix, vector, rows, columns, vectorSize);
  for (int i = 0; i < rows; i++) {
    EXPECT_EQ(sequentialResult[i], parallelResult[i]);
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
