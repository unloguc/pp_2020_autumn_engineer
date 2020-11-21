// Copyright 2020 Ognev Denis

#include "../../modules/task_2/ognev_d_vertical_matrix_vector_multiplication/multiplication.h"
#include <mpi.h>
#include <random>
#include <ctime>

int* getRandomMatrix(int rows, int columns) {
  std::mt19937 gen;
  std::uniform_int_distribution<> uid(0, 10);
  int* matrix = new int[rows * columns];
  gen.seed(static_cast<unsigned int>(time(0)));
  for (int i = 0; i < rows * columns; i++) {
    matrix[i] = uid(gen);
  }
  return matrix;
}

int* getRandomVector(int size) {
  std::mt19937 gen;
  std::uniform_int_distribution<> uid(0, 10);
  int* vector = new int[size];
  gen.seed(static_cast<unsigned int>(time(0)) + 1);
  for (int i = 0; i < size; i++) {
    vector[i] = uid(gen);
  }
  return vector;
}

int* sequentialMultiplication1(int* matrix, int* vector, int rows, int columns, int vectorSize) {  // HORIZONTAL
  if (columns != vectorSize) {
    throw "Number of matrix's columns are not equal to number of vector's rows";
  }
  if (matrix == nullptr || vector == nullptr) {
    throw "Matrix/vector can't be empty";
  }
  int* resultVector = new int[rows];
  for (int i = 0; i < rows; i++) {
    resultVector[i] = 0;
  }
  for (int i = 0; i < rows * columns; i++) {
    resultVector[i / columns] += matrix[i] * vector[i % columns];
  }
  return resultVector;
}

int* sequentialMultiplication2(int* matrix, int* vector, int rows, int columns, int vectorSize) {  // VERTICAL
  if (columns != vectorSize) {
    throw "Number of matrix's columns are not equal to number of vector's rows";
  }
  if (matrix == nullptr || vector == nullptr) {
    throw "Matrix/vector can't be empty";
  }
  int* resultVector = new int[rows];
  for (int i = 0; i < rows; i++) {
    resultVector[i] = 0;
  }
  for (int i = 0; i < rows * columns; i++) {
    resultVector[i % rows] += matrix[i * columns - (rows * columns - 1) * (i / rows)] * vector[i / rows];
  }
  return resultVector;
}

int* sequentialMultiplication3(int* matrix, int* vector, int rows, int columns, int vectorSize) {  // VERTICAL #2
  if (columns != vectorSize) {
    throw "Number of matrix's columns are not equal to number of vector's rows";
  }
  if (matrix == nullptr || vector == nullptr) {
    throw "Matrix/vector can't be empty";
  }
  int* resultVector = new int[rows];
  for (int i = 0; i < rows; i++) {
    resultVector[i] = 0;
  }
  for (int i = 0; i < columns; i++) {
    for (int j = i; j < rows * columns; j += columns) {
      resultVector[j / columns] += matrix[j] * vector[i];
    }
  }
  return resultVector;
}

int* parallelMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize) {  // PARALLEL VERTICAL
  if (columns != vectorSize) {
    throw "Number of matrix's columns are not equal to number of vector's rows";
  }
  if (matrix == nullptr || vector == nullptr) {
    throw "Matrix/vector can't be empty";
  }

  int proc_num, proc_rank;
  int* resultVector = new int[rows];
  int* result = new int[rows];

  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

  for (int i = 0; i < rows; i++) {
    resultVector[i] = 0;
  }
  for (int i = proc_rank; i < columns; i += proc_num) {
    for (int j = i; j < rows * columns; j += columns) {
      resultVector[j / columns] += matrix[j] * vector[i];
    }
  }
  MPI_Reduce(resultVector, result, rows, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(result, rows, MPI_INT, 0, MPI_COMM_WORLD);

  return result;
}
