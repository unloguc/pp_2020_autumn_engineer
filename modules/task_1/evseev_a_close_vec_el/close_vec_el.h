// Copyright 2020 Evseev Alexander
#ifndef MODULES_TASK_1_EVSEEV_A_CLOSE_VEC_EL_CLOSE_VEC_EL_H_
#define MODULES_TASK_1_EVSEEV_A_CLOSE_VEC_EL_CLOSE_VEC_EL_H_


#include <vector>
#include <climits>


std::vector<int> getRandomVector(int  sz);
int getParallelOperations(const std::vector<int> global_vec);
int getSequentialOperations(const std::vector<int> v);

#endif  // MODULES_TASK_1_EVSEEV_A_CLOSE_VEC_EL_CLOSE_VEC_EL_H_

