// Copyright 2020 Kustova Anastasiya
#ifndef MODULES_TASK_2_KUSTOVA_A_JACOBY_JACOBY_H_
#define MODULES_TASK_2_KUSTOVA_A_JACOBY_JACOBY_H_

#include <vector>
#include <string>

double Distance(double *X_Old, double *X_New, int n);

double *Iterations(int n, double *X_Old, double *X_New, double *Bloc_X,
    double *BRecv, double *ARecv, int GlobalRowNo, int amountRowBloc, int rank);
double *Iteration_for_0_rank(int n, double *X_Old, double *Input_B, double *Bloc_XX,
    double *Input_A, int GlobalRowNo, int amountRowBloc, int size);

double* Parallel_Jacoby(double Input_A[], double Input_B[], int n);
double* Sequential_Jacoby(double Input_A[], double Input_B[], int n);
#endif  // MODULES_TASK_2_KUSTOVA_A_JACOBY_JACOBY_H_
