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
