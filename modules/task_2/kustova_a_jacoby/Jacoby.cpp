// Copyright 2020 Kustova Anastasiya
#include "../../../modules/task_2/kustova_a_jacoby/Jacoby.h"
#include <mpi.h>
#include <math.h>
#define MAX_ITERATIONS 100
double Distance(double *X_Old, double *X_New, int n) {
    int i;
    double max = 0;
    for (i = 0; i < n; i++) {
        if (my_abs(X_Old[i] - X_New[i]) > max)
        max = my_abs(X_Old[i] - X_New[i]);
    }
    return(max);
}
double my_abs(double x) {
    if (x > 0) {
        return x;
    } else {
        return -x;
    }
}
double *Iterations(int n, double *X_Old, double *X_New, double *Bloc_X, double *BRecv,
        double *ARecv, int GlobalRowNo, int amountRowBloc, int rank) {
    int index;
    for (int irow = 0; irow < n; irow++) {
        X_Old[irow] = X_New[irow];
    }

    for (int irow = 0; irow < amountRowBloc; irow++) {
        GlobalRowNo = (rank * amountRowBloc) + irow;
        Bloc_X[irow] = BRecv[irow];
        index = irow * n;

        for (int icol = 0; icol < n; icol++) {
            if (icol != GlobalRowNo) {
                Bloc_X[irow] -= X_Old[icol] * ARecv[index + icol];
            }
        }
        Bloc_X[irow] = Bloc_X[irow] / ARecv[index + GlobalRowNo];
    }
    return Bloc_X;
}

double *Iteration_for_0_rank(int n, double *X_Old, double *Input_B, double *Bloc_XX,
        double *Input_A, int GlobalRowNo, int amountRowBloc, int size) {
    int index;
    for (int irow = amountRowBloc * size; irow < n; irow ++) {
        GlobalRowNo = irow;
        Bloc_XX[irow] = Input_B[irow];
        index = irow * n;
        for (int icol = 0; icol < n; icol++) {
            if (icol != GlobalRowNo) {
                Bloc_XX[irow] -= X_Old[icol] * Input_A[index + icol];
            }
        }
        Bloc_XX[irow] = Bloc_XX[irow] / Input_A[index + GlobalRowNo];
    }
    return Bloc_XX;
}
