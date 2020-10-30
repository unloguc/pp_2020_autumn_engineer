// Copyright 2020 Evseev Alexander
#include <mpi.h>
#include <cmath>
#include <climits>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../../modules/task_1/evseev_a_close_vec_el/close_vect_el.h"


std::vector<int> getRandomVector(int sz) {
    std::mt19937 gen(static_cast<unsigned int>(time(0)));
    std::uniform_int_distribution<int> rand(-10000, 10000);
    std::vector<int> v(sz);
    for (int i = 0; i < sz; i++) { v[i] = rand(gen); }
    return v;
}

int getSequentialOperations(const std::vector<int> v) {
    int  sz = v.size();
    int variMin = INT_MAX;
    if (sz != 0) {
        for (int i = 0; i < static_cast<int>(sz) - 1; i++) {
            if (variMin > abs(v[i] - v[i + 1]))
                variMin = abs(v[i] - v[i + 1]);
        }
    }
    if (sz == 1) {
        return 0;
    }
    return variMin;

}

int getParallelOperations(std::vector<int> global_vec) {
    int comm_size, rank;
    int variMin = INT_MAX;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;
    int size = global_vec.size() - 1;
    int buf_size = size / comm_size;
    int res = size % comm_size;

    if (rank == 0) {
        if (buf_size >= 1) {
            for (int i = 1; i < comm_size; i++)
                MPI_Send(&global_vec[res] + buf_size * i, buf_size, MPI_INT, i, 0, MPI_COMM_WORLD);

        }
    }
    else {
        if (buf_size >= 1) {
            std::vector<int> t(buf_size + 1);
            MPI_Recv(&t[0], buf_size + 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int min = getSequentialOperations(t);
            MPI_Send(&min, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    if (rank == 0) {
        int q;
        std::vector<int> t(global_vec.begin(), global_vec.begin() + 1 + buf_size + res);
        variMin = getSequentialOperations(t);
        if (buf_size >= 1) {
            for (int i = 1; i < comm_size; i++) {
                MPI_Recv(&q, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                variMin = std::min(variMin, q);
            }
        }
    }
    return variMin;
}