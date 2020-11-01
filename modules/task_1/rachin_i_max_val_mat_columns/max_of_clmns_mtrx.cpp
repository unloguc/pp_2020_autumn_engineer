// Copyright 2020 Rachin Igor
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include "./max_of_clmns_mtrx.h"


std::vector<int> getRandomMatrix(int rows, int clmns) {
    if (rows <= 0 || clmns <= 0)
        return std::vector<int>();
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> mx(rows * clmns);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < clmns; j++)
            mx[i * clmns + j] = gen() % 100;
    return mx;
}

std::vector<int> getSequentialMaxOfClmns(std::vector<int> mx, int rows, int clmns) {
    if (rows <= 0 || clmns <= 0)
        return std::vector<int>();
    std::vector<int> maxValues(clmns);
    for (int j = 0; j < clmns; j++)
        maxValues[j] = mx[j];
    for (int  i = 1; i < rows; i++) 
        for (int j = 0; j < clmns; j++)
            if (mx[i * clmns + j] > maxValues[j])
                maxValues[j] = mx[i * clmns + j];
    return maxValues;
}

std::vector<int> getParallelMaxOfClmns(std::vector<int> mx, int rows, int clmns) {
    if (rows <= 0 || clmns <= 0)
        return std::vector<int>();
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int delta = clmns / size;
    int tail = clmns % size;
    std::vector<int> localMtrx;
    std::vector<int> localMain(delta * rows);
    std::vector<int> localTail(tail * rows);
    std::vector<int>  maxValues(clmns);  //result vector
    if (rank == 0) {
        localMtrx = mx;
        for (int proc = 1; proc < size; proc++)
            MPI_Send(&localMtrx[0] + proc * delta * rows, delta * rows, MPI_INT, proc, 0, MPI_COMM_WORLD);
    }
    if (rank == 0) 
    {
        localMain = std::vector<int>(localMtrx.begin(), localMtrx.begin() + delta*rows);
        if (tail != 0)
            localTail = std::vector<int>(localMtrx.end() - tail * rows, localMtrx.end());
    }
    else 
    {
        MPI_Status status;
        MPI_Recv(&localMain[0], delta*rows, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    localMain = getSequentialMaxOfClmns(localMain, rows, delta);
    if (rank == 0 && tail != 0)
        localTail = getSequentialMaxOfClmns(localTail, rows, tail);
    MPI_Reduce(&localMain[0], &maxValues[0], delta, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0 && tail != 0)
        for (int i = 0; i < tail; i++)
            maxValues[clmns - tail + i] = localTail[i];
    return maxValues;
}
