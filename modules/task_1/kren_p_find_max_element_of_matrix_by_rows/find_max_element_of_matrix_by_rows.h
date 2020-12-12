// Copyright 2020 Kren Polina
#ifndef MODULES_TASK_1_KREN_P_FIND_MAX_ELEMENT_OF_MATRIX_BY_ROWS_FIND_MAX_ELEMENT_OF_MATRIX_BY_ROWS_H_
#define MODULES_TASK_1_KREN_P_FIND_MAX_ELEMENT_OF_MATRIX_BY_ROWS_FIND_MAX_ELEMENT_OF_MATRIX_BY_ROWS_H_

#include <vector>
#include <string>

std::vector<int> getRandomMatrix(int  rows, int columns);
std::vector<int> getMaxInRows(const std::vector<int>& matrix, int  rows, int columns);
std::vector<int> getMaxInRowsParall(const std::vector<int>& matrix, int rows, int columns);

#endif  // MODULES_TASK_1_KREN_P_FIND_MAX_ELEMENT_OF_MATRIX_BY_ROWS_FIND_MAX_ELEMENT_OF_MATRIX_BY_ROWS_H_
