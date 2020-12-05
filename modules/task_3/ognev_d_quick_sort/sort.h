// Copyright 2020 Ognev Denis
#ifndef MODULES_TASK_3_OGNEV_D_QUICK_SORT_SORT_H_
#define MODULES_TASK_3_OGNEV_D_QUICK_SORT_SORT_H_

int* getRandomVector(int size);
void merge(int* array, int leftB, int split, int rightB);
void quickSort(int* array, int leftB, int rightB);
void quickSortWithMergeSequential(int* array, int size, int n);
int* quickSortWithMergeParallel(int* array, int size);

#endif  // MODULES_TASK_3_OGNEV_D_QUICK_SORT_SORT_H_
