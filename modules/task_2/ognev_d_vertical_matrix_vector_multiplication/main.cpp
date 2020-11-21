// Copyright 2020 Ognev Denis

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "../../modules/task_2/ognev_d_vertical_matrix_vector_multiplication/multiplication.h"

TEST(Sequential_Multiplication_Test, Throws_When_Matrix_Is_Empty) {
  int rows = 5, columns = 5, vectorSize = columns;
  int* matrix = nullptr;
  int* vector = getRandomVector(vectorSize);
  ASSERT_ANY_THROW(sequentialMultiplication2(matrix, vector, rows, columns, vectorSize));
}

TEST(Sequential_Multiplication_Test, Throws_When_Vector_Is_Empty) {
  int rows = 5, columns = 5, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = nullptr;
  ASSERT_ANY_THROW(sequentialMultiplication2(matrix, vector, rows, columns, vectorSize));
}

TEST(Sequential_Multiplication_Test, Throws_When_Number_Of_Matrix_Columns_Are_Not_Equal_To_Number_Of_Vector_Rows) {
  int rows = 5, columns = 5, vectorSize = 6;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  ASSERT_ANY_THROW(sequentialMultiplication2(matrix, vector, rows, columns, vectorSize));
}

TEST(Parallel_Multiplication_Test, Throws_When_Matrix_Is_Empty) {
  int rows = 5, columns = 5, vectorSize = columns;
  int* matrix = nullptr;
  int* vector = getRandomVector(vectorSize);
  ASSERT_ANY_THROW(parallelMultiplication(matrix, vector, rows, columns, vectorSize));
}

TEST(Parallel_Multiplication_Test, Throws_When_Vector_Is_Empty) {
  int rows = 5, columns = 5, vectorSize = columns;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = nullptr;
  ASSERT_ANY_THROW(parallelMultiplication(matrix, vector, rows, columns, vectorSize));
}

TEST(Parallel_Multiplication_Test, Throws_When_Number_Of_Matrix_Columns_Are_Not_Equal_To_Number_Of_Vector_Rows) {
  int rows = 5, columns = 5, vectorSize = 6;
  int* matrix = getRandomMatrix(rows, columns);
  int* vector = getRandomVector(vectorSize);
  ASSERT_ANY_THROW(parallelMultiplication(matrix, vector, rows, columns, vectorSize));
}

TEST(Multiplication_Test, Parallel_Equals_To_Sequential) {
  int rows = 5, columns = 6, vectorSize = columns;
  int* matrix1 = getRandomMatrix(rows, columns);
  int* vector1 = getRandomVector(vectorSize);
  int* sequentialResult = sequentialMultiplication2(matrix1, vector1, rows, columns, vectorSize);
  int* parallelResult = parallelMultiplication(matrix1, vector1, rows, columns, vectorSize);
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
