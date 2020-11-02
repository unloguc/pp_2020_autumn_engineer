// Copyright 2020 Kustova Anastasiya
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <stdio.h> 
#define MAX_ITERATIONS 100
using namespace std;

/*
std::vector<int> generateVector(int n) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(n);
    for (int i = 0; i < n; i++) { vec[i] = gen() % 100; }
    return vec;
}

int getLocalMax(std::vector<int> vec) {
    const int  len = vec.size();
    int current_elem = vec[0];
    for (int  i = 1; i < len; i++) {
        current_elem = std::max(current_elem, vec[i]);
    }
    return current_elem;
}

int getParallelMax(std::vector<int> vec, int len_vector) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int delta = len_vector / size;
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Send(&vec[0] + i * delta, delta,
                        MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    std::vector<int> local_vec(delta);
    if (rank == 0) {
        local_vec = std::vector<int>(vec.begin(), vec.begin() + delta);
    } else {
        MPI_Status status;
        MPI_Recv(&local_vec[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int total_sum = 0;
    int local_sum = getLocalMax(local_vec);
    MPI_Op op_code;
    op_code = MPI_MAX;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return total_sum;
}*/



double Distance(double *X_Old, double *X_New, int n)
{
    int index;
    double Sum;
    Sum = 0.0;
    for(index = 0; index < n; index++){
	Sum += (X_New[index] - X_Old[index]) * (X_New[index] - X_Old[index]);
    }
    return(Sum);
}

double* Parallel_Jacoby(int n, double eps)
{
    int size,rank,amountRowBloc, GlobalRowNo, index;
//    clock_t begin, finish;

n = 2;
double *Input_A, *Input_B, *ARecv, *BRecv, *Bloc_XX, *X_New, *X_Old, *Bloc_X;

MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

X_New = new double[n];

if(rank==0)
{

Input_A = new double[n * n];
Input_B = new double[n];
/*test1*/
Input_A[0] = 4; Input_A[1] = 2; Input_A[2] = 1; Input_A[3] = 3;
Input_B[0] = 1; Input_B[1] = -1;
/*test2
Input_A[0] = 10; Input_A[1] = 1; Input_A[2] = -1; Input_A[3] = 1;
Input_A[4] = 10; Input_A[5] = -1; Input_A[6] = -1; Input_A[7] = 1; Input_A[8] = 10;
Input_B[0] = 11; Input_B[1] = 10; Input_B[2] = 10;*/
/*test3
Input_A[0] = 115; Input_A[1] = -20; Input_A[2] = -75; Input_A[3] = 15;
Input_A[4] = -50; Input_A[5] = -5; Input_A[6] = 6; Input_A[7] = 2; Input_A[8] = 20;
Input_B[0] = 20; Input_B[1] = -40; Input_B[2] = 28;*/

}
//begin = clock();
MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
//MPI_Barrier(MPI_COMM_WORLD);
    amountRowBloc = n / size;	//how much to each proc
ARecv = new double[amountRowBloc * n];
BRecv = new double[amountRowBloc];

MPI_Scatter (Input_A, amountRowBloc * n, MPI_DOUBLE, ARecv, amountRowBloc * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
MPI_Scatter (Input_B, amountRowBloc, MPI_DOUBLE, BRecv, amountRowBloc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

Bloc_X = new double[n];
X_New = new double[n];
X_Old = new double[n];

for (int irow=0; irow < amountRowBloc; irow ++){
    Bloc_X[irow] = BRecv[irow];
}

MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);
int Iteration = 0;

for(int irow=amountRowBloc * size; irow<n; irow++){
	MPI_Allgather(&Input_B[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
}

Bloc_XX = new double[n];
do{
    for(int irow = 0; irow < n;irow++){
	X_Old[irow] = X_New[irow];
    }

    for(int irow = 0; irow < amountRowBloc; irow++){
	GlobalRowNo = (rank * amountRowBloc) + irow;
	Bloc_X[irow] = BRecv[irow];
	index = irow * n;

	for(int icol = 0; icol < n; icol++){
	    if (icol != GlobalRowNo){
		Bloc_X[irow] -= X_Old[icol] * ARecv[index + icol];
	    }
	}
	Bloc_X[irow] = Bloc_X[irow] / ARecv[index + GlobalRowNo];
    }
    MPI_Allgather(Bloc_X, amountRowBloc, MPI_DOUBLE, X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);

    if (rank == 0){
	for(int irow = amountRowBloc * size; irow < n; irow ++){
	    GlobalRowNo = irow;
	    Bloc_XX[irow] = Input_B[irow];
	    index = irow * n;
	    for(int icol = 0; icol < n; icol++){
		if (icol != GlobalRowNo){
		    Bloc_XX[irow] -= X_Old[icol] * Input_A[index + icol];
		}
	    }
	    Bloc_XX[irow] = Bloc_XX[irow] / Input_A[index + GlobalRowNo];
	}
    }

    for(int irow = amountRowBloc * size; irow < n; irow ++){
	MPI_Allgather(&Bloc_XX[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
    }
    Iteration++;
}while((Iteration < MAX_ITERATIONS) && (Distance(X_Old, X_New, n) >= eps));
//finish = clock();
return X_New;
}
