// Copyright 2020 Pasukhin Dmitry

#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>
#include "../../../modules/task_2/pasukhin_d_hypercube/hypercube.h"

MPI_Comm createHypercubeComm() {
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  bool isCorr = checkProc(size);
  if (!isCorr) {
    throw std::runtime_error("Invalid count of processes");
  }

  const int reorder = 1;
  MPI_Comm old_comm = MPI_COMM_WORLD, new_comm;

  int power = powerHyperCube(size);
  int* index = new int[size];
  for (int i = 0; i < size; ++i) {
    index[i] = power;
  }

  int edges_count = size * power;
  int* edges = new int[edges_count];
  for (int i = 0; i < size; ++i) {
    int* roude = new int[power];
    computeRoude(i, power, roude);
    for (int j = 0; j < power; ++j) {
      edges[i * power + j] = roude[j];
    }
    delete[] roude;
  }

  MPI_Graph_create(old_comm, size, index, edges, reorder, &new_comm);

  return new_comm;
}

int randPath(int min, int max) {
  if (min < 0)
    min = 0;
  if (max < 0)
    max = 0;
  std::mt19937 generator;
  std::uniform_int_distribution<> distribution{ min, max };
  return distribution(generator);
}

int powerHyperCube(const int _size) {
  int power = 1, count = 0;
  while (power < _size) {
    power <<= 1;
    count++;
  }

  return count;
}

void computeRoude(const int rank, const int power, int* collection) {
  for (int i = 1, j = 0; j < power; i <<= 1, j++) {
    if (rank & i)
      collection[j] = -i;
    else
      collection[j] = i;
  }
}


bool checkProc(const int size) {
  int pow = 1;
  for (int i = 0; i <= 10; ++i) {
    if (size == pow)
      return true;
    pow *= 2;
  }
  return false;
}
