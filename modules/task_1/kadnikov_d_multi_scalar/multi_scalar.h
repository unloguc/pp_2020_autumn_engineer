// Copyright 2020 Denis Kadnikov
#ifndef MODULES_TASK_1_KADNIKOV_D_MATRIX_MULTI_SCALAR_MULTI_SCALAR_H_
#define MODULES_TASK_1_KADNIKOV_D_MATRIX_MULTI_SCALAR_MULTI_SCALAR_H_

#include <vector>

std::vector<int> createRandomVector(const int v_size);
int getSequentialMultiScalar(const std::vector<int>& v1, const std::vector<int>& v2);
int getParallelMultiScalar(const std::vector<int>& v1, const std::vector<int>& v2);

#endif  // MODULES_TASK_1_KADNIKOV_D_MATRIX_MULTI_SCALAR_MULTI_SCALAR_H_
