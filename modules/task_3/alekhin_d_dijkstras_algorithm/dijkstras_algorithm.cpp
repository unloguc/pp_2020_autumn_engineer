// Copyright 2020 Alekhin Denis
#include <mpi.h>
#include <iostream>
#include <random>
#include <ctime>
#include <limits>
#include <vector>
#include "../../modules/task_3/alekhin_d_dijkstras_algorithm/dijkstras_algorithm.h"

void getRandomGraph(std::vector<int>* graph, int size) {
  std::mt19937 gen;
  gen.seed(static_cast<unsigned int>(time(0)));
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i == j) {
        graph->push_back(0);
      } else if (j < i) {
        graph->push_back((*graph)[j * size + i]);
      } else {
        graph->push_back(gen() % 5);
      }
    }
  }
}

void printGraph(const std::vector<int>* graph) {
  int size = sqrt(graph->size());

  for (size_t i = 0; i < graph->size(); i++) {
    std::cout << (*graph)[i] << " ";
    if ((i + 1) % size == 0) {
      std::cout << std::endl;
    }
  }
}

void printResult(const std::vector<int>* result) {
  for (size_t i = 0; i < result->size(); i++) {
    std::cout << (*result)[i] << " ";
  }
  std::cout << std::endl;
}


std::vector<int> getDijkstrasAlgorithmSequential(const std::vector<int>* graph, int root) {
  int graphSize = sqrt(graph->size());
  int maxInt = std::numeric_limits<int>::max();
  std::vector<bool> used(graphSize, false);
  std::vector<int> dist(graphSize, maxInt);
  dist[root] = 0;

  for (int i = 0; i < graphSize - 1; i++) {
    int vertex = -1;
    for (int j = 0; j < graphSize; j++) {
      if (!used[j] && (vertex == -1 || dist[j] < dist[vertex])) {
        vertex = j;
      }
    }

    if (dist[vertex] == maxInt) {
      break;
    }
    used[vertex] = true;

    for (int k = 0; k < graphSize; k++) {
      if ((*graph)[vertex * graphSize + k] != 0 &&
        dist[vertex] + (*graph)[vertex * graphSize + k] < dist[k]) {
        dist[k] = dist[vertex] + (*graph)[vertex * graphSize + k];
      }
    }
  }

  return dist;
}

std::vector<int> getDijkstrasAlgorithmParallel(const std::vector<int>* graph, int root) {
  int size, rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int graphSize = sqrt(graph->size());
  int maxInt = std::numeric_limits<int>::max();
  std::vector<bool> used(graphSize, false);
  std::vector<int> dist(graphSize, maxInt);
  dist[root] = 0;

  std::vector<int> local_graph(graphSize * (graphSize / size));
  MPI_Scatter(&(*graph)[(graphSize % size) * graphSize], (graphSize / size) * graphSize,
    MPI_INT, &local_graph[0], (graphSize / size) * graphSize, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    local_graph.insert(
      local_graph.begin(),
      graph->begin(),
      graph->begin() + (graphSize % size) * graphSize);
  }

  int localDelta;
  if (rank == 0) {
    localDelta = 0;
  } else {
    localDelta = graphSize % size + (graphSize / size) * rank;
  }

  struct {
    int value;
    int index;
  }local_vertex, global_vertex;

  int localGraphSize = local_graph.size() / graphSize;
  for (int i = 0; i < graphSize - 1; i++) {
    local_vertex.value = -1;
    local_vertex.index = -1;
    for (int j = localDelta; j < localGraphSize + localDelta; j++) {
      if (!used[j] && (local_vertex.index == -1 || dist[j] < dist[local_vertex.index])) {
        local_vertex.index = j;
        local_vertex.value = dist[local_vertex.index];
      }
    }

    if (local_vertex.index == -1) {
      local_vertex.value = maxInt;
    }
    MPI_Allreduce(&local_vertex, &global_vertex, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);
    if (global_vertex.index == -1 || dist[global_vertex.index] == maxInt) {
      break;
    }
    used[global_vertex.index] = true;

    for (int k = 0; k < localGraphSize; k++) {
      if (local_graph[global_vertex.index + graphSize * k] != 0 &&
        dist[global_vertex.index] + local_graph[global_vertex.index + graphSize * k] < dist[k + localDelta]) {
        dist[k + localDelta] = dist[global_vertex.index] + local_graph[global_vertex.index + graphSize * k];
      }
    }

    if (rank == 0) {
      dist.resize(graphSize % size + graphSize / size);
      std::vector<int> recv(graphSize / size);
      for (int i = 1; i < size; i++) {
        MPI_Recv(&recv[0], graphSize / size,
          MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        dist.insert(dist.end(), recv.begin(), recv.end());
      }
    } else {
      for (int i = 1; i < size; i++) {
        if (rank == i) {
          MPI_Send(&dist[graphSize % size + rank * (graphSize / size)], graphSize / size,
            MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
      }
    }
    MPI_Bcast(&dist[0], dist.size(), MPI_INT, 0, MPI_COMM_WORLD);
  }

  return dist;
}
