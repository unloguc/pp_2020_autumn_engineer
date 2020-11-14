// Copyright 2020 Kryukov Sergey
#ifndef MODULES_TASK_2_KRYUKOV_S_TOPOLOGY_STAR_TOPOLOGY_STAR_H_
#define MODULES_TASK_2_KRYUKOV_S_TOPOLOGY_STAR_TOPOLOGY_STAR_H_

#include <vector>
#include <string>

void initialVectorNodes(std::vector<int>& inputV);
MPI_Comm createTopologyStar(std::vector<int> MasNodes);
void createMasEdges(std::vector<int> inputV, std::vector<int>& edges, std::vector<int>& index);
int starSend(void *buf, int count, MPI_Datatype datatype, int dest, int send_tag, MPI_Comm comm);
int starRecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);

#endif  // MODULES_TASK_2_KRYUKOV_S_TOPOLOGY_STAR_TOPOLOGY_STAR_H_
