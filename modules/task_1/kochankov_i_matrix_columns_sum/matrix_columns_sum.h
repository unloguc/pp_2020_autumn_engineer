// Copyright 2020 Kochankov Ilya

#ifndef MODULES_TASK_1_KOCHANKOV_I_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_
#define MODULES_TASK_1_KOCHANKOV_I_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_

#include <vector>

using std::vector;

vector<vector<int>> get_rand_matrix(int x, int y);
vector<int> sequential_operations(const vector<vector<int>>& matrix);
vector<int> parallel_sum(const vector<vector<int>>& matrix);

#endif  // MODULES_TASK_1_KOCHANKOV_I_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_
