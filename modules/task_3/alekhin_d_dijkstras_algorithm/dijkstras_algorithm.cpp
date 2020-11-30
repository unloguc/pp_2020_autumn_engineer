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
  std::vector<bool> global_used(graphSize, false);
  std::vector<int> global_dist(graphSize, maxInt);
  global_dist[root] = 0;

  std::vector<int> local_graph(graphSize * (graphSize / size));
  MPI_Scatter(
    &graph[graphSize % size + graphSize],
    graphSize / size,
    MPI_INT,
    &local_graph[0],
    graphSize / size,
    MPI_INT,
    0,
    MPI_COMM_WORLD);
  if (rank == 0) {
    local_graph.insert(
      local_graph.begin(), 
      graph->begin(), 
      graph->begin() + graphSize % size);
  }
  /*
  std::vector<bool> local_used(local_graph.size(), false);
  std::vector<int> local_dist(local_graph.size(), maxInt);
  for (size_t i = 0; i < local_graph.size(); i++) {
    int vertex = -1;
    for (size_t j = 0; j < local_graph.size(); j++) {
      if (!local_used[j] && (vertex == -1 || dist[j] < dist[vertex])) {
        vertex = j;
      }
    }
  }*/
}

