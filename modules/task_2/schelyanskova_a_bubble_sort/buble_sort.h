// Copyright 2020 Schelyanskova Anastasiya
#ifndef MODULES_TASK_2_SCHELYANSKOVA_A_BUBBLE_SORT_BUBLE_SORT_H_
#define MODULES_TASK_2_SCHELYANSKOVA_A_BUBBLE_SORT_BUBLE_SORT_H_

#include <mpi.h>
#include <vector>

std::vector<int> getRandomVector(const int& size);
std::vector<int> sequentialBubbleSort(const std::vector<int>& vec);
std::vector<int> parallelOddEvenSort(std::vector<int> vec, const int& size);
int computePartner(const int& iter, const int& rank);



#endif  // MODULES_TASK_2_SCHELYANSKOVA_A_BUBBLE_SORT_BUBLE_SORT_H_
