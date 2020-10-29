// Copyright 2020 Mikhail Baulin
#ifndef MODULES_TASK_2_BAULIN_M_MATRIX_VECTORT_MUL_MATRIX_VECTORT_MUL_MPI_H_
#define MODULES_TASK_2_BAULIN_M_MATRIX_VECTORT_MUL_MATRIX_VECTORT_MUL_MPI_H_

#include <vector>

std::vector<int> getMatrixWithRandomNumbers(int rows, int cols);
std::vector<int> getSequentialOperations(std::vector<int> mat, int rows, int cols, std::vector<int> vec);
std::vector<int> getParallelOperations(std::vector<int> mat, int rows, int cols, std::vector<int> vec);

#endif  // MODULES_TASK_2_BAULIN_M_MATRIX_VECTORT_MUL_MATRIX_VECTORT_MUL_MPI_H_
