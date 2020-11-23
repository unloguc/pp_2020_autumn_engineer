// Copyright 2020 Stoicheva Darya
#ifndef MODULES_TASK_2_STOICHEVA_D_ALLREDUCE_MPI_H_
#define MODULES_TASK_2_STOICHEVA_D_ALLREDUCE_MPI_H_

#include <mpi.h>
#include <vector>
#include <string>

int my_MPI_Reduce(void* send_data, void* recv_data, int count, MPI_Datatype datatype,
    MPI_Op op, int root, MPI_Comm comm);
int my_MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, 
    MPI_Op op, MPI_Comm comm);

int getSequentialOperations(std::vector<int> vec, std::string ops);

#define DEBUG_PRINT

#endif  // MODULES_TASK_2_STOICHEVA_D_ALLREDUCE_MPI_H_
