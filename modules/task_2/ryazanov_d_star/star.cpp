// Copyright 2020 Ryazanov Dmitry
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../../modules/task_2/ryazanov_d_star/star.h"


MPI_Comm star_comm_create() {
  int numtasks;

  MPI_Comm star_comm;

  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  int *index = new int[numtasks];
  int *edges = new int[2 * numtasks - 1];

  index[0] = 1;
  for (int i = 1; i < numtasks; i++) {
    index[i] = 2;
  }

  for (int num = 0; num < numtasks - 1; num++) {
    edges[num] = num + 1;
  }

  for (int num = numtasks - 1; num < 2 * numtasks - 1; num++) {
    edges[num] = 0;
  }

  MPI_Graph_create(MPI_COMM_WORLD, numtasks, index, edges, 1, &star_comm);
  return star_comm;
}
