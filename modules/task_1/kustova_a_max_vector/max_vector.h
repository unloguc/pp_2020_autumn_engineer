// Copyright 2020 Kustova Anastasiya
#ifndef MODULES_TASK_1_KUSTOVA_A_MAX_VECTOR_MAX_VECTOR_H_
#define MODULES_TASK_1_KUSTOVA_A_MAX_VECTOR_MAX_VECTOR_H_

#include <vector>
#include <string>

std::vector<int> getRandomVector(int  sz);
int getParallelOperations(std::vector<int> global_vec,
                          int count_size_vector, std::string ops);
int getSequentialOperations(std::vector<int> vec, std::string ops);

#endif  // MODULES_TASK_1_KUSTOVA_A_MAX_VECTOR_MAX_VECTOR_H_
