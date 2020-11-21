// Copyright 2020 Kamskov Eugene
#ifndef MODULES_TASK_2_KAMSKOV_E_MULTI_SCALAR_MULTI_SCALAR_H_
#define MODULES_TASK_2_KAMSKOV_E_MULTI_SCALAR_MULTI_SCALAR_H_

#include <vector>

std::vector<int> getRandomMatrix(int rows, int columns);
std::vector<int> getParallelOperations(std::vector<int> global_vec, std::vector<int> sec_global_vec,
    int rows);
std::vector<int> getSequentialOperations(std::vector<int> vec, std::vector<int> sec_vec, int size_row);
std::vector<int> getRes(std::vector<int> vec, std::vector<int> sec_vec, int size_row);
#endif  // PP_2020_AUTUMN_ENGINEER_MASTER_MODULES_TASK_2_KAMSKOV_E_MATRIX_MULTI_FOR_A_MATRIX_MULTI_FOR_A_H_
