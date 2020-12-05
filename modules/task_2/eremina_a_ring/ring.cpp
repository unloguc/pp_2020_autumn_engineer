// Copyright 2020 Eremina Alena
#include "../../../modules/task_2/eremina_a_ring/ring.h"

MPI_Comm topologyRing(int* order) {
    int ProcRank, ProcNum;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int *index = new int[ProcNum];
    for (int proc = 0; proc < ProcNum; proc++) {
        index[proc] = 2 * (proc + 1);
    }

    int *edges = new int[ProcNum * 2];
    if (order != NULL) {
        int left, right;
        for (int i = 0; i < ProcNum; i++) {
            if (i == 0) {
                left = order[ProcNum-1];
            } else {
                left = order[i - 1];
            }
            if (i == ProcNum - 1) {
                right = order[0];
            } else {
                right = order[i + 1];
            }
            edges[2 * order[i]] = left;
            edges[2 * order[i] + 1] = right;
        }
    } else {
        for (int i = 0; i < ProcNum; i++) {
            edges[(2 + (i * 2)) % (ProcNum * 2)] = i;
            edges[(ProcNum * 2 - 1 + i * 2) % (ProcNum * 2)] = i;
        }
    }

    MPI_Comm RingComm;
    MPI_Graph_create(MPI_COMM_WORLD, ProcNum, index, edges, true, &RingComm);

    return RingComm;
}
