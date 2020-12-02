// Copyright 2020 Stoicheva Darya
#ifndef MODULES_TASK_2_STOICHEVA_D_ALLREDUCE_ALLREDUCE_MPI_H_
#define MODULES_TASK_2_STOICHEVA_D_ALLREDUCE_ALLREDUCE_MPI_H_

#include <mpi.h>
#include <vector>
#include <string>
#include <iostream>


int my_MPI_Reduce(const void* send_data, void* recv_data, int count, MPI_Datatype datatype,
    MPI_Op op, int root, MPI_Comm comm);
int my_MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
    MPI_Op op, MPI_Comm comm);

template<typename T>
extern T getSequentialOperations2(const std::vector<T> vector, const MPI_Op op);

template<typename T>
extern T getSequentialOperations(const std::vector<T> vec, const MPI_Op op);

void print_vector(const std::vector<int> vector, const size_t size, const std::string prefix = {},
    const std::string label = {}, std::ostream &out = std::cout);

template<typename T>
void print_vector(const std::vector<T> vector, const size_t size, const std::string prefix = {},
    const std::string label = {}, std::ostream &out = std::cout);

int swap_0_with_root(bool swap_required, int proc_id, int root_id);

// #define DEBUG_PRINT

#endif  // MODULES_TASK_2_STOICHEVA_D_ALLREDUCE_ALLREDUCE_MPI_H_

