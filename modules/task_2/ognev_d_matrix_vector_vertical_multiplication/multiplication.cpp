// Copyright 2020 Ognev Denis

#include "../../modules/task_2/ognev_d_matrix_vector_vertical_multiplication/multiplication.h"
#include <mpi.h>
#include <random>
#include <ctime>

int* getRandomMatrix(int rows, int columns) {
  if (rows < 1 || columns < 1) {
    throw "Number of rows or columns can't be less than 1";
  }
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
  if (size < 1) {
    throw "Size can't be less than 1";
  }
  std::mt19937 gen;
  std::uniform_int_distribution<> uid(0, 10);
  int* vector = new int[size];
  gen.seed(static_cast<unsigned int>(time(0)) + 1);
  for (int i = 0; i < size; i++) {
    vector[i] = uid(gen);
  }
  return vector;
}

int* matrixTransposition(int* matrix, int rows, int columns) {
  if (matrix == nullptr) {
    throw "Matrix can't be empty";
  }
  if (rows < 1 || columns < 1) {
    throw "Number of rows or columns can't be less than 1";
  }
  int* resultMatrix = new int[rows * columns];
  for (int i = 0; i < rows * columns; i++) {
    resultMatrix[i] = 0;
  }
  for (int i = 0; i < rows * columns; i++) {
    resultMatrix[rows * (i % columns) + i / columns] = matrix[i];
  }
  return resultMatrix;
}

int* horizontalSequentialMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize) {
  if (rows < 1 || columns < 1 || vectorSize < 1) {
    throw "Number of rows or columns can't be less than 1";
  }
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

int* parallelNotTransposedMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize) {
  if (rows < 1 || columns < 1 || vectorSize < 1) {
    throw "Number of rows or columns can't be less than 1";
  }
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

int* sequentialTransposedMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize) {
  if (rows < 1 || columns < 1 || vectorSize < 1) {
    throw "Number of rows or columns can't be less than 1";
  }
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
    resultVector[i % rows] += matrix[i] * vector[i / rows];
  }
  return resultVector;
}

int* parallelTransposedMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize) {
  if (rows < 1 || columns < 1 || vectorSize < 1) {
    throw "Number of rows or columns can't be less than 1";
  }
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
  MPI_Bcast(matrix, rows * columns, MPI_INT, 0, MPI_COMM_WORLD);
  for (int i = proc_rank; i < columns; i += proc_num) {
    for (int j = 0; j < rows; j++) {
      resultVector[j % rows] += matrix[i * rows + j] * vector[i];
    }
  }
  MPI_Reduce(resultVector, result, rows, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(result, rows, MPI_INT, 0, MPI_COMM_WORLD);

  return result;
}
