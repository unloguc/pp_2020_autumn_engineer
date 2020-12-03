// Copyright 2020 Sandalov Konstantin
#ifndef MODULES_TASK_2_SANDALOV_K_TORUS_TORUS_H_
#define MODULES_TASK_2_SANDALOV_K_TORUS_TORUS_H_

#include <mpi.h>
#include <vector>


bool createTorusTopology(MPI_Comm* torusComm, int gridHeight = 1, int gridWidth = 1);
bool freeTorusTopology(MPI_Comm* torusComm, bool topoCreated);
int createRoadMap(MPI_Comm* torusComm, int start, int finish);
int takeRandomPoint(int max = 0);
int sendMessageInTorus(MPI_Comm* torusComm, std::vector<int>* message, int src, int dst);

#endif  // MODULES_TASK_2_SANDALOV_K_TORUS_TORUS_H_
