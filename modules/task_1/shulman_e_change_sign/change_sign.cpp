// Copyright 2020 Shulman Egor
#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../../modules/task_1/shulman_e_change_sign/change_sign.h"


std::vector<int> RandomVector(int size_vector) {
    std::mt19937 random;
    random.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vector(size_vector);
    for (int  i = 0; i < size_vector; i++)
        vector[i] = random() % 400 - 200;
    return vector;
}

int SequentialPart(std::vector<int> vector, int size_vector) {
    if (size_vector == 0)
        return 0;

    int LocalCounter = 0;
    for (int i = 0; i < size_vector - 1; i++) {
        if ((vector[i] * vector[i + 1]) == 0) {
            if (vector[i] < 0 || vector[i + 1] < 0)
                LocalCounter++;
        }
        if ((vector[i] * vector[i + 1]) < 0)
            LocalCounter++;
    }
    return LocalCounter;
}

int ParallelPart(std::vector<int> vector, int size_vector) {
    if (size_vector == 0)
        return 0;

    int size, rank, GeneralCounter = 0, LocalCounter = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int block = size_vector / size;
    int remainder = size_vector % size;

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Send(&vector[0] + i * block + remainder - 1, block + 1,  MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    if (rank == 0) {
        std::vector<int> LocalVector(block + remainder);
        LocalVector = std::vector<int>(vector.begin(), vector.begin() + block + remainder);
        LocalCounter = SequentialPart(LocalVector, static_cast<int>(LocalVector.size()));
    } else {
        MPI_Status status;
        std::vector<int> LocalVector(block + 1);
        MPI_Recv(&LocalVector[0], block+1 , MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        LocalCounter = SequentialPart(LocalVector, static_cast<int>(LocalVector.size()));
    }

    MPI_Reduce(&LocalCounter, &GeneralCounter, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    return GeneralCounter;
}
