// Copyright 2020 Kustova Anastasiya
#ifndef MODULES_TASK_1_KUSTOVA_A_MAX_VECTOR_MAX_VECTOR_H_
#define MODULES_TASK_1_KUSTOVA_A_MAX_VECTOR_MAX_VECTOR_H_

#include <vector>
#include <string>

std::vector<int> getRandomVector(int  n);
int getParallelOperations(std::vector<int> vec,
                          int len);
int getSequentialOperations(std::vector<int> vec);

#endif  // MODULES_TASK_1_KUSTOVA_A_MAX_VECTOR_MAX_VECTOR_H_
