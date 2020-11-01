// Copyright 2020 Kustova Anastasiya
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <stdio.h> 
//#include "./Jacoby.h"
#include <time.h>
#define MAX_ITERATIONS 10
using namespace std;
double Distance(std::vector<double> X_Old, std::vector<double> X_New, int n);
int main(int argc, char** argv){
    int size,rank,n, amountRowBloc, GlobalRowNo;
//    clock_t begin, finish;
cout << 0;
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
cout << 1;
n = 2;


double eps=0.00001;
int index;
std::vector<double> Input_A(n * n);
std::vector<double> Input_B(n);
if(rank==0)
{

    //+++++++
//    Input_A = {10, 1, -1, 1, 10, -1, -1, 1, 10};
//    Input_B = {11, 10, 10};
////+++++++
//    Input_A = {115, -20, -75, 15, -50, -5, 6, 2, 20};
 //   Input_B = {20, -40, 28};
/////
    Input_A = {4, 2, 1, 3};
    Input_B = {1, -1};
cout << 2;
    amountRowBloc = n / size;	//how much to each proc
}
//begin = clock();
MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
MPI_Barrier(MPI_COMM_WORLD);

std::vector<double> ARecv(n * amountRowBloc);
std::vector<double> BRecv(amountRowBloc);
cout << 3;
MPI_Scatter (&Input_A, amountRowBloc * n, MPI_DOUBLE, &ARecv, amountRowBloc * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
MPI_Scatter (&Input_B, amountRowBloc, MPI_DOUBLE, &BRecv, amountRowBloc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

std::vector<double> Bloc_X(n);
std::vector<double> X_New(n);
std::vector<double> X_Old(n);
std::vector<double> Bloc_XX(n);
cout << 4;

for (int irow=0; irow < amountRowBloc; irow ++){
    Bloc_X[irow] = BRecv[irow];
}

MPI_Allgather(&Bloc_X, amountRowBloc, MPI_DOUBLE, &X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);
int Iteration = 0;
cout << 5;
for(int irow=amountRowBloc * size; irow<n; irow++){
	MPI_Allgather(&Input_B[irow], 1, MPI_DOUBLE, &X_New[irow], 1, MPI_DOUBLE, MPI_COMM_WORLD);
}

std::vector<double> BlocXX(n);
do{
    for(int irow = 0; irow < n;irow++){
	X_Old[irow] = X_New[irow];
    }
cout << 6;
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
    MPI_Allgather(&Bloc_X, amountRowBloc, MPI_DOUBLE, &X_New, amountRowBloc, MPI_DOUBLE, MPI_COMM_WORLD);
cout << 7;
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
cout << 8;
//Output vector
if (rank == 0) {
    for(int irow = 0; irow < n; irow ++){
	cout << X_New[irow] << endl;
    }
}
    MPI_Finalize();
return 0;
}


double Distance(std::vector<double> X_Old, std::vector<double> X_New, int n)
{
    int index;
    double Sum;
    Sum = 0.0;
    for(index = 0; index < n; index++){
	Sum += (X_New[index] - X_Old[index]) * (X_New[index] - X_Old[index]);
    }
    return(Sum);
}
/*
TEST(Parallel_Operations_MPI, Test_Max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    const int len = 1001;
    if (rank == 0) {
        vec = generateVector(len);
    }
    int parallel_max = getParallelMax(vec, len);
    if (rank == 0) {
        int usual_max = getLocalMax(vec);
        ASSERT_EQ(usual_max, parallel_max);
    }
}

TEST(Parallel_Operations_MPI, Test_can_find_local_max) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec = {3, 4, 9, -6};
    if (rank == 0) {
        int usual_max = getLocalMax(vec);
        ASSERT_EQ(9, usual_max);
    }
}

TEST(Parallel_Operations_MPI, Test_can_gen_random_vector) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int len = 100;
    if (rank == 0) {
        ASSERT_NO_THROW(generateVector(len));
    }
}

TEST(Parallel_Operations_MPI, Test_throw_gen_vector_with_negative_length) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> vec;
    const int len = -20;
    if (rank == 0) {
        ASSERT_ANY_THROW(generateVector(len));
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}*/
