// Copyright 2020 Alexey Nekravtsev
#ifndef MODULES_TASK_3_NEKRAVTSEV_A_SIMPSON_SIMPSON_H_
#define MODULES_TASK_3_NEKRAVTSEV_A_SIMPSON_SIMPSON_H_

#include <mpi.h>

double Sequential_SimpsonForDouble(double (*f)(double x, double y), double a1, double b1,
  double a2, double b2, int n1, int n2);

double Parallel_SimpsonForDouble(double (*f)(double x, double y), double a1, double b1,
  double a2, double b2, int n1, int n2);

double func(double x, double y);

#endif  // MODULES_TASK_3_NEKRAVTSEV_A_SIMPSON_SIMPSON_H_
