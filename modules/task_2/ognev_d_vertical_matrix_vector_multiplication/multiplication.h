// Copyright 2020 Ognev Denis
#ifndef MODULES_TASK_2_OGNEV_D_VERTICAL_MATRIX_VECTOR_MULTIPLICATION_MULTIPLICATION_H_
#define MODULES_TASK_2_OGNEV_D_VERTICAL_MATRIX_VECTOR_MULTIPLICATION_MULTIPLICATION_H_

int* getRandomMatrix(int rows, int columns);
int* getRandomVector(int size);
int* sequentialMultiplication1(int* matrix, int* vector, int rows, int columns, int vectorSize);
int* sequentialMultiplication2(int* matrix, int* vector, int rows, int columns, int vectorSize);
int* sequentialMultiplication3(int* matrix, int* vector, int rows, int columns, int vectorSize);
int* parallelMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize);

#endif  // MODULES_TASK_2_OGNEV_D_VERTICAL_MATRIX_VECTOR_MULTIPLICATION_MULTIPLICATION_H_
