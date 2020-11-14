// Copyright 2020 Pasukhin Dmitry

#ifndef MODULES_TASK_2_PASUKHIN_D_HYPERCUBE_HYPERCUBE_H_
#define MODULES_TASK_2_PASUKHIN_D_HYPERCUBE_HYPERCUBE_H_

#include <mpi.h>

MPI_Comm createHypercubeComm();

int randPath(int min, int max);
int powerHyperCube(const int _size);
void computeRoude(const int rank, const int power, int* collection);
bool checkProc(const int size);

#endif  // MODULES_TASK_2_PASUKHIN_D_HYPERCUBE_HYPERCUBE_H_
