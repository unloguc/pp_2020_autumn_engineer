// Copyright 2020 Egorov Kirill
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../../modules/task_2/egorov_k_circle_comm/circle_comm.h"

int circle_comm_create() {
    int numtasks;

    MPI_Comm circle_comm;

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    int *ind = new int[numtasks];
    int *edg = new int[2 * numtasks];

    for (int i = 0; i < numtasks; i++) {
         ind[i] = (i + 1) * 2;
    }

    for (int num = 0; num < numtasks; num++) {
        int a = num - 1;
        int b = num + 1;

        if (a >= 0 && b < numtasks) {
            edg[num * 2] = a;
            edg[num * 2 + 1] = b;
        } else {
            if (a < 0) {
                edg[num] = numtasks - 1;
                edg[num + 1] = b;
            }
            if (b >= numtasks) {
                edg[num * 2] = a;
                edg[num * 2 + 1] = 0;
            }
        }
    }

    MPI_Graph_create(MPI_COMM_WORLD, numtasks, ind, edg, 1, &circle_comm);
    return circle_comm;
}
