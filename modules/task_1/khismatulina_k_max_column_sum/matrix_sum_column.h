// Copyright 2020 Khismatulina Karina
#ifndef MODULES_TASK_1_KHISMATULINA_K_MAX_COLUMN_SUM_MATRIX_SUM_COLUMN_H_
#define MODULES_TASK_1_KHISMATULINA_K_MAX_COLUMN_SUM_MATRIX_SUM_COLUMN_H_

#include <vector>
#include <string>

std::vector<int> getMatrix(int line, int column);
std::vector<int> transposedMatrix(std::vector<int> matrix, int line, int column);
int getMaxColumnSumSequential(std::vector<int> matrix, int line, int column);
int getMaxColumnSumParallel(std::vector<int> matrix, int line, int column);

#endif  // MODULES_TASK_1_KHISMATULINA_K_MAX_COLUMN_SUM_MATRIX_SUM_COLUMN_H_"

