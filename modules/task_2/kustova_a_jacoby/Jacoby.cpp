// Copyright 2020 Kustova Anastasiya
#include "../../../modules/task_2/kustova_a_jacoby/Jacoby.h"
#include <mpi.h>
#include <math.h>
#define MAX_ITERATIONS 100
double Distance(double *X_Old, double *X_New, int n) {
    int index;
    double Sum;
    Sum = 0.0;
    for (index = 0; index < n; index++) {
        Sum += (X_New[index] - X_Old[index]) * (X_New[index] - X_Old[index]);
    }
    return(Sum);
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


double* Parallel_Jacoby(double Input_A[], double Input_B[], int n) {
    int size, rank, amountRowBloc, GlobalRowNo, index;
    double *ARecv, *BRecv, *Bloc_XX, *X_New, *X_Old, *Bloc_X;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    X_New = new double[n];

    amountRowBloc = n / size;
    ARecv = new double[amountRowBloc * n];
    BRecv = new double[amountRowBloc];

    MPI_Scatter(Input_A, amountRowBloc * n, MPI_DOUBLE, ARecv, amountRowBloc * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(Input_B, amountRowBloc, MPI_DOUBLE, BRecv, amountRowBloc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    Bloc_X = new double[n];
    X_New = new double[n];
    X_Old = new double[n];

    for (int irow = 0; irow < amountRowBloc; irow ++) {
        Bloc_X[irow] = BRecv[irow];
    }

    MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);

    int Iteration = 0;
    for (int irow = amountRowBloc * size; irow < n; irow++) {
        MPI_Allgather(&Input_B[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
    }

    Bloc_XX = new double[n];
    do {
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
        MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);

        if (rank == 0) {
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
        }
        for (int irow = amountRowBloc * size; irow < n; irow ++) {
            MPI_Allgather(&Bloc_XX[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        Iteration++;
    } while ((Iteration < MAX_ITERATIONS) && (Distance(X_Old, X_New, n) >= 0.0001));
// finish = clock();
    return X_New;
}


double* Sequential_Jacoby(double Input_A[], double Input_B[], int n) {
    double *X_New, *X_Old;
    int index = 0, Iteration = 0, GlobalRowNo;
    X_New = new double[n];
    X_Old = new double[n];

    do {
        for (int irow = 0; irow < n; irow++) {
            X_Old[irow] = X_New[irow];
        }
        for (int irow = 0; irow < n; irow ++) {
            GlobalRowNo = irow;
            X_New[irow] = Input_B[irow];
            index = irow * n;
            for (int icol = 0; icol < n; icol++) {
                if (icol != GlobalRowNo) {
                   X_New[irow] -= X_Old[icol] * Input_A[index + icol];
                }
            }
        X_New[irow] = X_New[irow] / Input_A[index + GlobalRowNo];
        }
        Iteration++;
    }while((Iteration < MAX_ITERATIONS) && (Distance(X_Old, X_New, n) >= 0.0001));
    return X_New;
}
