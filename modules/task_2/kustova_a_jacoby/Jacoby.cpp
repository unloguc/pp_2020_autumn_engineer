// Copyright 2020 Kustova Anastasiya
#include "../../../modules/task_2/kustova_a_jacoby/Jacoby.h"
#include <mpi.h>
#include <vector>
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

std::vector<double> Parallel_Jacoby(std::vector<double> A, std::vector<double> B, int n, double eps) {
    int size, rank, amountRowBloc, GlobalRowNo;
    double *ARecv, *BRecv, *Bloc_XX, *X_New, *X_Old, *Bloc_X, *Input_A, *Input_B;
    std::vector<double> res(n);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    amountRowBloc = n / size;    // how much to each proc
    Input_A = new double[n * n];
    Input_B = new double[n];
    for (int i = 0; i < n * n; i++) {
        Input_A[i] = A[i];
        if (i < n) {
            Input_B[i] = B[i];
        }
    }
    ARecv = new double[amountRowBloc * n];
    BRecv = new double[amountRowBloc];

    MPI_Scatter(Input_A, amountRowBloc * n, MPI_DOUBLE, ARecv, amountRowBloc * n, MPI_DOUBLE, 0,     MPI_COMM_WORLD);
    MPI_Scatter(Input_B, amountRowBloc, MPI_DOUBLE, BRecv, amountRowBloc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    Bloc_X = new double[n];
    X_New = new double[n];
    X_Old = new double[n];

    for (int irow=0; irow < amountRowBloc; irow ++) {
        Bloc_X[irow] = BRecv[irow];
    }

    MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);
    int Iteration = 0;

    for (int irow = amountRowBloc * size; irow < n; irow++) {
        MPI_Allgather(&Input_B[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
    }

    Bloc_XX = new double[n];
    do {
        Bloc_X = Iterations(n, X_Old, X_New, Bloc_X, BRecv, ARecv, GlobalRowNo, amountRowBloc, rank);
        MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);

        if (rank == 0) {
            Bloc_XX = Iteration_for_0_rank(n, X_Old, Input_B, Bloc_XX, Input_A, GlobalRowNo,  amountRowBloc, size);
        }

        for (int irow = amountRowBloc * size; irow < n; irow ++) {
            MPI_Allgather(&Bloc_XX[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        Iteration++;
    }while((Iteration < MAX_ITERATIONS) && (Distance(X_Old, X_New, n) >= eps));
// finish = clock();
    for (int i = 0; i < n; i++) {
        res[i] = X_New[i];
    }
    return res;
}
