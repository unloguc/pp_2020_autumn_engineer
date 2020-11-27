// Copyright 2020 Krasilnikov Alexey

#include <mpi.h>
#include <iostream>
#include <ctime>
#include <random>
#include <algorithm>
#include <vector>
#include <utility>
#include "../../../modules/task_2/krasilnikov_a_bubble_sort/bubble_sort.h"


std::vector<int> getRandomVector(const size_t vector_size) {
  std::mt19937 generator;
  std::vector<int> ans(vector_size, 0);
  for (auto &symbol : ans) {
    symbol = generator() % 100000;
  }
  return ans;
}

void sortedArraySequential(int *array, const size_t array_size) {
  for (size_t i = array_size - 1; i > 0; --i) {
    for (size_t j = 0; j < i; ++j) {
      if (array[j] > array[j + 1]) {
        std::swap(array[j], array[j + 1]);
      }
    }
  }
}

void mergeArrays(int *array1, int *array2, const size_t array_size1, const size_t array_size2) {
  std::vector<int> ans;
  size_t pointer1 = 0, pointer2 = 0;
  while ((pointer1 < array_size1) && (pointer2 < array_size2)) {
    if (array1[pointer1] < array2[pointer2]) {
      ans.push_back(array1[pointer1++]);
    } else {
      ans.push_back(array2[pointer2++]);
    }
  }
  while (pointer1 < array_size1) {
    ans.push_back(array1[pointer1++]);
  }
  while (pointer2 < array_size2) {
    ans.push_back(array2[pointer2++]);
  }
  for (size_t i = 0; i < array_size1; ++i) {
    array1[i] = ans[i];
  }
  for (size_t i = 0; i < array_size2; ++i) {
    array2[i] = ans[array_size1 + i];
  }
}

void sortedArrayParallel(int *array, const int array_size) {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size > array_size) {
    if (rank == 0) {
      sortedArraySequential(array, array_size);
    }
    return;
  }
  MPI_Status status;
  int size_block = array_size / size;
  int delta = array_size % size;
  int k = 0;
  int *sendcounts = new int[size];
  int *displs = new int[size];
  for (int i = 0; i < size; ++i) {
    if (i < delta) {
      sendcounts[i] = size_block + 1;
    } else {
      sendcounts[i] = size_block;
    }
    displs[i] = k;
    k += sendcounts[i];
  }
  std::vector<int> local_vector;
  std::vector<int> neighbour_vector;
  if (rank < delta) {
    local_vector.resize(size_block + 1);
    MPI_Scatterv(array, sendcounts, displs, MPI_INT, local_vector.data(), size_block + 1, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    local_vector.resize(size_block);
    MPI_Scatterv(array, sendcounts, displs, MPI_INT, local_vector.data(), size_block, MPI_INT, 0, MPI_COMM_WORLD);
  }
  sortedArraySequential(local_vector.data(), local_vector.size());
  for (int i = 0; i < size; ++i) {
    if (i % 2 == 1) {
      if (rank % 2 == 1) {
        if (rank < size - 1) {
          int local_vector_size = static_cast<int>(local_vector.size());
          MPI_Send(&local_vector_size, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
          MPI_Send(local_vector.data(), local_vector_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
          int neighbour_vector_size;
          MPI_Recv(&neighbour_vector_size, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
          neighbour_vector.resize(neighbour_vector_size);
          MPI_Recv(neighbour_vector.data(), neighbour_vector_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
          mergeArrays(local_vector.data(), neighbour_vector.data(), local_vector_size, neighbour_vector_size);
        }
      } else if (rank > 0) {
        int local_vector_size = static_cast<int>(local_vector.size());
        MPI_Send(&local_vector_size, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        MPI_Send(local_vector.data(), local_vector_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        int neighbour_vector_size;
        MPI_Recv(&neighbour_vector_size, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        neighbour_vector.resize(neighbour_vector_size);
        MPI_Recv(neighbour_vector.data(), neighbour_vector_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        mergeArrays(neighbour_vector.data(), local_vector.data(), neighbour_vector_size, local_vector_size);
      }
    } else {
      if (rank % 2 == 0) {
        if (rank < size - 1) {
          int local_vector_size = static_cast<int>(local_vector.size());
          MPI_Send(&local_vector_size, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
          MPI_Send(local_vector.data(), local_vector_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
          int neighbour_vector_size;
          MPI_Recv(&neighbour_vector_size, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
          neighbour_vector.resize(neighbour_vector_size);
          MPI_Recv(neighbour_vector.data(), neighbour_vector_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
          mergeArrays(local_vector.data(), neighbour_vector.data(), local_vector_size, neighbour_vector_size);
        }
      } else {
        int local_vector_size = static_cast<int>(local_vector.size());
        MPI_Send(&local_vector_size, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        MPI_Send(local_vector.data(), local_vector_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        int neighbour_vector_size;
        MPI_Recv(&neighbour_vector_size, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        neighbour_vector.resize(neighbour_vector_size);
        MPI_Recv(neighbour_vector.data(), neighbour_vector_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        mergeArrays(neighbour_vector.data(), local_vector.data(), neighbour_vector_size, local_vector_size);
      }
    }
  }
  MPI_Gatherv(local_vector.data(), sendcounts[rank], MPI_INT, array, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
}
