// Copyright 2020 Voronin Aleksey
#ifndef MODULES_TASK_1_VORONIN_A_MULTI_SCALAR_MULTI_SCALAR_H_
#define MODULES_TASK_1_VORONIN_A_MULTI_SCALAR_MULTI_SCALAR_H_

#include <vector>

std::vector<int> getRandomVector(int  size);
int getParallelOperations(std::vector<int> global_vec, std::vector<int> second_global_vec,
                          int count_size_vector);
int getSequentialOperations(std::vector<int> vec, std::vector<int> second_vec);

#endif  // MODULES_TASK_1_VORONIN_A_MULTI_SCALAR_MULTI_SCALAR_H_
