// Copyright 2020 Sandalov Konstantin

#include <limits.h>
#include <random>
#include <vector>

#include "../../../modules/task_2/sandalov_k_torus/torus.h"


bool createTorusTopology(MPI_Comm* torusComm, int gridHeight, int gridWidth) {
    int ProcRank, ProcNum;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    const int dims[] = {gridHeight, gridWidth};
    const int periods[] = {true, true};
    const int reorder = true;

    int mpiCartCreated = MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, torusComm);
    if (mpiCartCreated != MPI_SUCCESS) return false;

    return (*torusComm != MPI_COMM_NULL);
}

bool freeTorusTopology(MPI_Comm* torusComm, bool topoCreated) {
    if (topoCreated) {
        int mpiCommFree = MPI_Comm_free(torusComm);
        return (mpiCommFree == MPI_SUCCESS);
    }
    return false;
}

int createRoadMap(MPI_Comm* torusComm, int start, int finish) {
    int startCoords[2];
    int finCoords[2];

    if (start == finish) return -1;

    int startCoordsRet = MPI_Cart_coords(*torusComm, start, 2, startCoords);
    int finCoordsRet = MPI_Cart_coords(*torusComm, finish, 2, finCoords);
    if (finCoordsRet != MPI_SUCCESS || startCoordsRet != MPI_SUCCESS) return -2;

    int dims[2];
    int periods[2];
    int coords[2];
    int cartGet = MPI_Cart_get(*torusComm, 2, dims, periods, coords);
    if (cartGet != MPI_SUCCESS) return -1;

    int stride[] = {0, 0};

    for (int i = 0; i < 2; ++i) {
        if (startCoords[i] != finCoords[i]) {
            if (startCoords[i] < finCoords[i]) {
                int shift = finCoords[i] - startCoords[i];
                int cycleShift = dims[i] - shift;
                if (cycleShift < shift) {
                    stride[i] = -1;
                } else {
                    stride[i] = 1;
                }
            } else {
                int shift = startCoords[i] - finCoords[i];
                int cycleShift = dims[i] - shift;
                if (cycleShift < shift) {
                    stride[i] = 1;
                } else {
                    stride[i] = -1;
                }
            }
            break;
        }
    }
    int nextStep = -1;
    int curCoords[2];
    curCoords[0] = startCoords[0];
    curCoords[1] = startCoords[1];

    for (int i = 0; i < 2; ++i) {
        if (stride[i] != 0) {
            curCoords[i] = startCoords[i] + stride[i];
            MPI_Cart_rank(*torusComm, curCoords, &nextStep);
        }
    }
    return nextStep;
}

int takeRandomPoint(int max) {
    const int min = 0;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distrib(min, max - 1);
    return distrib(generator);
}

int sendMessageInTorus(MPI_Comm* torusComm, std::vector<int>* message, int src, int dst) {
    int result = 0;
    int stopper = 0;
    int procRank, procNum;
    MPI_Comm_size(*torusComm, &procNum);
    MPI_Comm_rank(*torusComm, &procRank);
    MPI_Status status;

    if (procRank == src) {
            if (src == dst) {
                for (int i = 0; i < procNum; ++i) {
                    if (i != procRank) MPI_Send(&stopper, 1, MPI_INT32_T, i, procNum + 1, *torusComm);
                }
                result = 1;
            } else {
                int bufSize = message->size();
                int next = createRoadMap(torusComm, src, dst);
                MPI_Send(message->data(), bufSize, MPI_INT32_T, next, dst, *torusComm);
                MPI_Recv(&stopper, 1, MPI_INT32_T, dst, procNum + 1, *torusComm, &status);
            }
    } else {
        int count;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, *torusComm, &status);
        MPI_Get_count(&status, MPI_INT32_T, &count);
        message->resize(count);
        MPI_Recv(message->data(), count, MPI_INT32_T, MPI_ANY_SOURCE, MPI_ANY_TAG, *torusComm, &status);

        if (status.MPI_TAG == procNum + 1) {
            result = -1;
        } else if (status.MPI_TAG == procRank) {
            for (int i = 0; i < procNum; ++i) {
                    if (i != procRank) MPI_Send(&stopper, 1, MPI_INT32_T, i, procNum + 1, *torusComm);
            }
            result = 1;
        } else {
            int next = createRoadMap(torusComm, procRank, status.MPI_TAG);
            MPI_Send(message->data(), count, MPI_INT32_T, next, status.MPI_TAG, *torusComm);
            MPI_Recv(&stopper, 1, MPI_INT32_T, status.MPI_TAG, procNum + 1, *torusComm, &status);
        }
    }
    MPI_Barrier(*torusComm);
    return result;
}
