// Copyright 2020 Evseev Alexander
#ifndef MODULES_TASK_1_EVSEEV_A_CLOSE_VEC_EL_H_
#define MODULES_TASK_1_EVSEEV_A_CLOSE_VEC_EL_H_


#include <vector>


std::vector<int> getRandomVector(int  sz);
int getParallelOperations(std::vector<int> global_vec);
int getSequentialOperations(std::vector<int> v);

#endif  // MODULES_TASK_1_EVSEEV_A_CLOSE_VEC_EL_H_