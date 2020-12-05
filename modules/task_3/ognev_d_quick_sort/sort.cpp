// Copyright 2020 Ognev Denis

#include "../../modules/task_3/ognev_d_quick_sort/sort.h"
#include <mpi.h>
#include <iostream>
#include <random>
#include <ctime>
#include <utility>
#include <vector>

int* getRandomVector(int size) {
  if (size < 1) {
    throw "Size can't be less than 1";
  }
  std::mt19937 gen;
  std::uniform_int_distribution<> uid(0, 10000);
  int* vector = new int[size];
  gen.seed(static_cast<unsigned int>(time(0)));
  for (int i = 0; i < size; i++) {
    vector[i] = uid(gen);
  }
  return vector;
}

void merge(int* array, int leftB, int split, int rightB) {
  if (array == nullptr) {
    throw "Array can't be empty";
  }
  int pos1 = leftB, pos2 = split + 1, pos3 = 0;
  int* temp = new int[rightB - leftB + 1];

  while (pos1 <= split && pos2 <= rightB) {
    if (array[pos1] < array[pos2]) {
      temp[pos3++] = array[pos1++];
    } else {
      temp[pos3++] = array[pos2++];
      }
  }

  while (pos2 <= rightB) {
    temp[pos3++] = array[pos2++];
  }

  while (pos1 <= split) {
    temp[pos3++] = array[pos1++];
  }

  for (pos3 = 0; pos3 < rightB - leftB + 1; pos3++) {
    array[leftB + pos3] = temp[pos3];
  }
}

void quickSort(int* array, int leftB, int rightB) {
  if (array == nullptr) {
    throw "Array can't be empty";
  }
  int l = leftB, r = rightB;
  int median = array[(l + r) / 2];
  while (array[l] < median) {
    l++;
  }
  while (array[r] > median) {
    r--;
  }
  if (l <= r) {
    std::swap(array[l++], array[r--]);
  }
  if (leftB < r) {
    quickSort(array, leftB, r);
  }
  if (rightB > l) {
    quickSort(array, l, rightB);
  }
}

void quickSortWithMergeSequential(int* array, int size, int n) {
  if (array == nullptr) {
    throw "Array can't be empty";
  }
  if (size < 1) {
    throw "Size can't be less than 1";
  }
  if (n < 1) {
    throw "Number of segments can't be less than 1";
  }
  std::vector<int> v;
  for (int i = 0; i < n; i++) {
    quickSort(array, i * size / n, (i + 1) * size / n - 1);
    v.push_back((i + 1) * size / n - 1);
  }
  for (int i = 0; i < n - 1; i++) {
    merge(array, 0, v[i], v[i + 1]);
  }
}

int* quickSortWithMergeParallel(int* array, int size) {
  if (array == nullptr) {
    throw "Array can't be empty";
  }
  if (size < 1) {
    throw "Size can't be less than 1";
  }
  int proc_num, proc_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

  int sendcount = ((proc_rank + 1) * size / proc_num) - proc_rank * size / proc_num;
  int* rbuf = new int[sendcount];
  int* result = new int[size];

  std::vector<int> v;

  int* sendcounts = new int[proc_num];
  int* displs = new int[proc_num];

  for (int i = 0; i < proc_num; i++) {
    sendcounts[i] = ((i + 1) * size / proc_num - 1) - i * size / proc_num + 1;
    displs[i] = i * size / proc_num;
  }

  MPI_Scatterv(&array[0], sendcounts, displs, MPI_INT, rbuf, sendcount, MPI_INT, 0, MPI_COMM_WORLD);

  quickSort(rbuf, 0, sendcount - 1);

  MPI_Gatherv(rbuf, sendcount, MPI_INT, result, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

  if (proc_rank == 0) {
    for (int i = 0; i < proc_num; i++) {
      v.push_back((i + 1) * size / proc_num - 1);
    }
    for (int i = 0; i < proc_num - 1; i++) {
      merge(result, 0, v[i], v[i + 1]);
    }
  }
  MPI_Bcast(result, size, MPI_INT, 0, MPI_COMM_WORLD);

  return result;
}
