// Copyright 2018 Nesterov Alexander
#include <iostream>
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include "ops_mpi.h"


std::string getRandomString(int sz)
{
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::string m_string(sz, '\0');
    for (auto& symbol : m_string){ symbol = static_cast<char>(gen() % 100); }
    return m_string;
}

int countFrequencySec(const std::string& m_string, char symbol, int size)
{
    int frequencyCount = 0;
    for (int i = 0; i < size; ++i)
    {
        if (m_string[i] == symbol) ++frequencyCount;
    }
    return frequencyCount;
}

int countFrequencyPar(const std::string& global_string, char symbol, int string_size)
{
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (string_size < size)
    {
        if (rank == 0) return
                    countFrequencySec(global_string, symbol, string_size);
        else return 0;
    }
    const int delta = string_size / size;
    int realSize;
    if (rank == size - 1) realSize = string_size - (size - 1) * delta;
    else realSize = delta;
 
    if (rank == 0)
    {
        for (int proc = 1; proc < size - 1; ++proc)
        {
            MPI_Send(&global_string[proc * delta], delta,
                        MPI_CHAR, proc, 0, MPI_COMM_WORLD);
        }
        MPI_Send(&global_string[(size - 1) * delta], string_size - (size - 1) * delta,
                        MPI_CHAR, size - 1, 0, MPI_COMM_WORLD);
        
    }
    int global_sum = 0;
    int local_sum = 0;
    std::string local_string;
    local_string.resize(delta);
    if (rank == 0)
    {
        local_sum = countFrequencySec(global_string, symbol, delta);
    }
    else
    {
        MPI_Status status;
        MPI_Recv(&local_string[0], realSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        local_sum = countFrequencySec(local_string, symbol, realSize);
    }

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    return global_sum;
}
