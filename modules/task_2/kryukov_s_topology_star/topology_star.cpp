// Copyright 2020 Kryukov Sergey
#include <mpi.h>
#include <random>
#include <ctime>
#include <algorithm>
#include "windows.h"
#include "../../../modules/task_2/kryukov_s_topology_star/topology_star.h"

void initialVectorNodes(std::vector<int>& inputV) {
	for (int j = 0; j < inputV.size(); ++j)
		inputV[j] = j;
}

void createMasEdges(std::vector<int> inputV, std::vector<int>& edges, std::vector<int>& index) {
	index.resize(inputV.size());
	edges.resize((inputV.size() - 1) * 2);
	index[0] = inputV.size() - 1;
	for (int j = 1; j < inputV.size(); ++j) {
		index[j] = index[0] + j;
		if(j != inputV.size())
	    edges[j-1] = j;
	}
	for (int i = inputV.size(); i < (inputV.size() - 1) * 2; i++) {
		edges[i] = 0;
	}	
}
 
MPI_Comm createTopologyStar(std::vector<int> MasNodes) {
	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	std::vector<int> listEdges;
	std::vector<int> outArc;
	initialVectorNodes(MasNodes);
	createMasEdges(MasNodes, listEdges, outArc);
	int * index = new int[outArc.size()];
	int * edges = new int[listEdges.size()];
	for (int i = 0; i < outArc.size(); ++i) {
		index[i] = outArc[i];
	}
	for (int i = 0; i < listEdges.size(); ++i) {
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

