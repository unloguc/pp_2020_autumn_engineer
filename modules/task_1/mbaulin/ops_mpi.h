#ifndef MODULES_TASK_1_BAULIN_M_MATRIX_MAX_MPI_H_
#define MODULES_TASK_1_BAULIN_M_MATRIX_MAX_MPI_H_

#include <vector>
#include <string>

int** fillMatrixWithRandomNumbers(int** mat, int rows, int cols);
int findMax(int** mat, int rows, int cols);
int getParallelOperations(int** mat, int rows, int cols);

#endif  // MODULES_TASK_1_BAULIN_M_MATRIX_MAX_MPI_H_