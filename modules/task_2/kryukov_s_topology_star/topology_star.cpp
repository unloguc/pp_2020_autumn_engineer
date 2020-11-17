// Copyright 2020 Kryukov Sergey
#include <mpi.h>
#include <random>
#include <ctime>
#include <vector>
#include <algorithm>
#include "../../../modules/task_2/kryukov_s_topology_star/topology_star.h"

std::vector<int> initialVectorNodes(std::vector<int> inputV, int Size) {
    for (int j = 0; j < Size; ++j)
        inputV[j] = j;
    return inputV;
}

std::vector<int> createMasEdges(std::vector<int> inputV, int Size) {
    std::vector<int> edges;
    //if(size>1)
    edges.resize((Size - 1) * 2);
    for (int j = 1; j < Size; ++j) {
        if (j != Size)
            edges[j - 1] = j;
    }
    for (int i = Size;
        i < (Size - 1) * 2; i++) {
        edges[i] = 0;
    }
	//else(
    return edges;
}

std::vector<int> createMasIndex(std::vector<int> inputV, int Size) {
    std::vector<int> index;
    index.resize(Size);
    index[0] = Size - 1;
    //if(size > 1)
    for (int j = 1; j < Size; ++j) {
        index[j] = index[0] + j;
    }
    return index;
}


MPI_Comm createTopologyStar(MPI_Comm comm, std::vector<int> MasNodes) {
    int rank;
    int size;
	int reorder = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    std::vector<int> nodes;
    std::vector<int> listEdges;
    std::vector<int> outArc;
    nodes = initialVectorNodes(MasNodes, size);
    listEdges = createMasEdges(nodes, size);
    outArc = createMasIndex(nodes, size);

    int * index = new int[size];
    int * edges = new int[(size - 1) * 2];
    for (int i = 0; i < size; ++i) {
        index[i] = outArc[i];
    }
    for (int i = 0; i < static_cast<unsigned int>(listEdges.size()); ++i) {
        edges[i] = listEdges[i];
    }
    MPI_Comm StarComm;
    MPI_Graph_create(comm, size, index, edges, reorder, &StarComm);

    //delete[] index;
    //delete[] edges;
    return StarComm;
}

int starSend(void *buf, int count, MPI_Datatype datatype, int dest, int send_tag, MPI_Comm comm) {
    return MPI_Send(buf, count, datatype, dest, send_tag, comm);
}

int starRecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm,
    MPI_Status *status) {
    return MPI_Recv(buf, count, datatype, source, tag, comm, status);
}
