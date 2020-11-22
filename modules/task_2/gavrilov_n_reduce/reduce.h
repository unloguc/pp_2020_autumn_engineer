// Copyright 2020 Gavrilov Nikita
#ifndef MODULES_TASK_2_GAVRILOV_N_REDUCE_REDUCE_H_
#define MODULES_TASK_2_GAVRILOV_N_REDUCE_REDUCE_H_

#include <mpi.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <random>

template <typename T>
MPI_Datatype getMPIDataType() {
    if (std::is_same<T, int>::value)
        return MPI_INT;
    else if (std::is_same<T, float>::value)
        return MPI_FLOAT;
    else if (std::is_same<T, double>::value)
        return MPI_DOUBLE;

    throw "Type not supported.";
}

template <typename T>
T getRandomVar() {
    std::mt19937 rnd;
    rnd.seed(static_cast<unsigned int>(time(0)));
    if (std::is_same<T, int>::value)
        return rnd() % 20000 - 10000;
    else if (std::is_same<T, float>::value || std::is_same<T, double>::value)
        return getRandomVar<int>() + rnd() % 100 * 0.01;
    throw "Type not supported.";
}

template <typename T>
std::vector<T> getRandomVector(size_t size) {
    std::vector<T> result(size);

    for (size_t i = 0; i < size; i++) {
        result[i] = getRandomVar<T>();
    }

    return result;
}

template <typename T>
T operationSum(T a, T b) {
    return a + b;
}

template <typename T>
T operationMax(T a, T b) {
    return std::max(a, b);
}

template <typename T>
T operationMin(T a, T b) {
    return std::min(a, b);
}

template <typename T>
T operationMply(T a, T b) {
    return a * b;
}

template <typename T>
void operation(T* arr1, T* arr2, int count, MPI_Op op) {
    T(*func)(T a, T b);

    if (op == MPI_SUM)
        func = operationSum;
    else if (op == MPI_MAX)
        func = operationMax;
    else if (op == MPI_MIN)
        func = operationMin;
    else if (op == MPI_PROD)
        func = operationMply;
    else
        throw "Operation not supported.";

    for (int i = 0; i < count; i++) {
        arr1[i] = func(arr1[i], arr2[i]);
    }

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
}

void operation(MPI_Datatype datatype, void* arr1, void* arr2, int count, MPI_Op op);

void* createBuf(MPI_Datatype datatype, int count);

void copy(MPI_Datatype datatype, int count, void* dest, void* src);

void deleteBuf(MPI_Datatype datatype, void* buf);

void reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);

#endif  // MODULES_TASK_2_GAVRILOV_N_REDUCE_REDUCE_H_
