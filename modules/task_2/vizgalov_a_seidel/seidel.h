// Copyright 2020 Vizgalov Anton
#ifndef MODULES_TASK_2_VIZGALOV_A_SEIDEL_SEIDEL_H_
#define MODULES_TASK_2_VIZGALOV_A_SEIDEL_SEIDEL_H_

#include <mpi.h>

#include <cmath>


double calculatePrecision(int size, double* res, double* prevRes);
void seidelSequential(int size, double** mat, double* vec, int numIterations, double* res, double* precision);
void seidelParallel(int size, double** mat, double* vec, int numIterations, double* res, double* precision);

#endif  // MODULES_TASK_2_VIZGALOV_A_SEIDEL_SEIDEL_H_
