// Copyright 2020 Ognev Denis
#ifndef MODULES_TASK_2_OGNEV_D_MATRIX_VECTOR_VERTICAL_MULTIPLICATION_MULTIPLICATION_H_
#define MODULES_TASK_2_OGNEV_D_MATRIX_VECTOR_VERTICAL_MULTIPLICATION_MULTIPLICATION_H_

int* getRandomMatrix(int rows, int columns);
int* getRandomVector(int size);
int* matrixTransposition(int* matrix, int rows, int columns);
int* horizontalSequentialMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize);
int* parallelNotTransposedMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize);
int* sequentialTransposedMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize);
int* parallelTransposedMultiplication(int* matrix, int* vector, int rows, int columns, int vectorSize);


#endif  // MODULES_TASK_2_OGNEV_D_MATRIX_VECTOR_VERTICAL_MULTIPLICATION_MULTIPLICATION_H_
