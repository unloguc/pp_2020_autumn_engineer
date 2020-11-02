// Copyright 2020 Kucheryavyh Yan
#ifndef MODULES_TASK_1_KUCHERYAVYH_Y_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_
#define MODULES_TASK_1_KUCHERYAVYH_Y_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_

#include <vector>

std::vector<int> createRandomMatrix(const int rows, const int cols);
std::vector<int> getSequintialColumnsSum(const std::vector<int> Matrix, int rows, int cols);
std::vector<int> getParallelColumnsSum(const std::vector<int> Matrix, int rows, int cols);

#endif  // MODULES_TASK_1_KUCHERYAVYH_Y_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_
