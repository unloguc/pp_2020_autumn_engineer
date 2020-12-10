
// Copyright 2018 Nesterov Alexander
#ifndef MODULES_TASK_3_FROLOVA_O_RADIX_SORT_SIMPLE_MERGE_UPDATE_RADIX_SORT_SIMPLE_MERGE_H_
#define MODULES_TASK_3_FROLOVA_O_RADIX_SORT_SIMPLE_MERGE_UPDATE_RADIX_SORT_SIMPLE_MERGE_H_
#include <mpi.h>
#include <vector>
#include <string>

std::vector<int> getRandomVector(int size);
std::vector<int> checkVector(std::vector<int> vec);
int countRadix(int num);
int maxVal(std::vector<int> vec);
std::vector<int> merge(std::vector<int> vec1, std::vector<int> vec2);
std::vector<int> radixSort(std::vector<int> vec, int radix);
std::vector<int> ParallelRadixSort(std::vector<int> vec, int size1);
void difOfTime(int size1);
void timeOfRadixSort(int size1);

#endif  // MODULES_TASK_3_FROLOVA_O_RADIX_SORT_SIMPLE_MERGE_UPDATE_RADIX_SORT_SIMPLE_MERGE_H_
