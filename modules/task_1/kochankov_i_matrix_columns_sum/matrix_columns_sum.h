// Copyright 2020 Kochankov Ilya

#ifndef MODULES_TASK_1_KOCHANKOV_I_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_
#define MODULES_TASK_1_KOCHANKOV_I_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_

#include <vector>

using std::vector;
#define Matrix(T) vector<vector<T>>

Matrix(int) get_rand_matrix(int x, int y);
vector<int> sequential_operations(const Matrix(int)& matrix);
vector<int> parallel_sum(const Matrix(int)& matrix);

#endif  // MODULES_TASK_1_KOCHANKOV_I_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_
