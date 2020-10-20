//
// Created by OgRob on 19.10.2020.
//

#include <ctime>
#include <mpi.h>
#include <random>
#include <iostream>
#include "./letters_amount.h"

std::string getRandomString(int size) {
  std::mt19937 gen;
  const std::string alphabet =
      "0Aa1Bb2Cc3Dd4Ee5Ff6GgHh7Ii8Jj9KkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
  std::string new_str;
  gen.seed(static_cast<unsigned int>(time(0)));
  for (std::size_t i = 0; i < (size_t)size; ++i) {
    new_str+= alphabet[gen() % 62 ];
  }
  //std::cout<<"randomstring_done"<<std::endl;
  return new_str;
}

int getSequentialOperations(std::string str) {
  int cnt = 0;
  for (std::size_t i = 0; i < (std::size_t)str.size(); ++i) {
    if ((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z')) {
      ++cnt;
    }
  }
  return cnt;
}

int getParallelOperations(std::string global_str, int size_str) {
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int global_cnt=0;
  const int delta = size_str / size;
  int remain = size_str % size;
  if (rank == 0) {
    int margin = 0;
    if (remain) {
      margin = 1;
    }
    for (std::size_t process = 1; process < (std::size_t)size; ++process) {
      if (remain - (int)process > 0) {
        MPI_Send(&global_str[0] + process * delta + margin, delta + 1,
                 MPI_CHAR, process, 0, MPI_COMM_WORLD);
        ++margin;
      }
      else {
        MPI_Send(&global_str[0] + process * delta + margin, delta,
                 MPI_CHAR, process, 0, MPI_COMM_WORLD);
      }
    }
  }
  std::string local_str;
  local_str.resize(delta);
  if (rank<=remain-1) {
    local_str.resize(delta+1);
  }
  if (rank == 0) {
    if (rank<=remain-1) {
      local_str = std::string(global_str.begin(), global_str.begin() + delta + 1);
    }
    else {
      local_str = std::string(global_str.begin(), global_str.begin() + delta);
    }

  } else {
    MPI_Status status;
    if (rank <= remain-1) {
      MPI_Recv(&local_str[0], delta + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
    } else {
      MPI_Recv(&local_str[0], delta, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
    }
  }
  int local_cnt = getSequentialOperations(local_str);
  MPI_Reduce(&local_cnt, &global_cnt, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  return global_cnt;
}