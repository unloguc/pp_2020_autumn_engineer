// Copyright 2020 Krasilnikov Alexey

#ifndef MODULES_TASK_2_KRASILNIKOV_A_BUBBLE_SORT_BUBBLE_SORT_H_
#define MODULES_TASK_2_KRASILNIKOV_A_BUBBLE_SORT_BUBBLE_SORT_H_

#include <vector>

std::vector<int> getRandomVector(const size_t vector_size);
void sortedArraySequential(int *array, const size_t array_size);
void mergeArrays(int *array1, int *array2, const size_t array_size1, const size_t array_size2);
void sortedArrayParallel(int *array, int size_array);


#endif  //  MODULES_TASK_2_KRASILNIKOV_A_BUBBLE_SORT_BUBBLE_SORT_H_
