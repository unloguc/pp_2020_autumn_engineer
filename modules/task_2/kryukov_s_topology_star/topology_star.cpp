// Copyright 2020 Kryukov Sergey
#include <mpi.h>
#include <random>
#include <ctime>
#include <vector>
#include <algorithm>
#include "../../../modules/task_2/kryukov_s_topology_star/topology_star.h"

std::vector<int> initialVectorNodes(std::vector<int> inputV) {
    for (int j = 0; j < static_cast<unsigned int>(inputV.size()); ++j)
        inputV[j] = j;
    return inputV;
}

std::vector<int> createMasEdges(std::vector<int> inputV) {
    std::vector<int> edges;
    edges.resize((inputV.size() - 1) * 2);
    for (int j = 1; j < inputV.size(); ++j) {
        if (j != inputV.size())
            edges[j - 1] = j;
    }
    for (int i = static_cast<unsigned int>(inputV.size()); i < (static_cast<unsigned int>(inputV.size()) - 1) * 2; i++) {
        edges[i] = 0;
    }
    return edges;
}

std::vector<int> createMasIndex(std::vector<int> inputV) {
    std::vector<int> index;
    index.resize(inputV.size());
    index[0] = inputV.size() - 1;
    for (int j = 1; j < static_cast<unsigned int>(inputV.size()); ++j) {
        index[j] = index[0] + j;
    }
    return index;
}


MPI_Comm createTopologyStar(std::vector<int> MasNodes) {
    int rank;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    std::vector<int> nodes;
    std::vector<int> listEdges;
    std::vector<int> outArc;
    nodes = initialVectorNodes(MasNodes);
    listEdges = createMasEdges(nodes);
    outArc = createMasIndex(nodes);

    int * index = new int[outArc.size()];
    int * edges = new int[listEdges.size()];
    for (int i = 0; i < static_cast<unsigned int>(outArc.size()); ++i) {
        index[i] = outArc[i];
    }
    for (int i = 0; i < static_cast<unsigned int>(listEdges.size()); ++i) {
        edges[i] = listEdges[i];
    }
    MPI_Comm StarComm;
    MPI_Graph_create(MPI_COMM_WORLD, MasNodes.size(), index, edges, 1, &StarComm);

    delete[] index;
    delete[] edges;
    return StarComm;
}

int starSend(void *buf, int count, MPI_Datatype datatype, int dest, int send_tag, MPI_Comm comm) {
    return MPI_Send(buf, count, datatype, dest, send_tag, comm);
}

int starRecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm,
    MPI_Status *status) {
    return MPI_Recv(buf, count, datatype, source, tag, comm, status);
}
