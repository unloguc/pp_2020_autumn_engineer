// Copyright 2020 Ermolin Dmitry
#include <mpi.h>
#include "../../../modules/task_2/ermolin_d_hypercube/hypercube.h"

int DimCount(int size) {
    int tmp = 1;
    int i;
    for (i = 0 ; size >= tmp * 2 ; i++) {
        tmp *= 2;
    }
    return i;
}

MPI_Comm HyperCube() {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm comm;
    int count = DimCount(size);
    int* dim = new int[count];
    int* period = new int[count];

    for (int i = 0; i < count; i++) {
        dim[i] = 2;
        period[i] = 1;
    }

    MPI_Cart_create(MPI_COMM_WORLD, count, dim, period, 0, &comm);
    delete[] dim;
    delete[] period;
    return comm;
}
