// Copyright 2020 Pasukhin Dmitry

#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>
#include "../../../modules/task_2/pasukhin_d_hypercube/hypercube.h"

MPI_Request globreq;

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

int computeSize(const int proc_size) {
  int add = proc_size % (sizeof(int) * 8) == 0 ? 0 : 1;
  return 1 + (proc_size / (sizeof(int) * 8) + add);
}

int reciver_nearby(const int _rank, const int _reciever, const int _power, const int* _dir) {
  int res = 0;
  int* far_dir = new int[_power] {0};
  for (int i = 0; i < _power; ++i) {
    int neigb = _rank + _dir[i];
    if (neigb == _reciever) {
      res = (((1 << 1) | 1) | i << 2);
      delete[] far_dir;
      return res;
    }
    for (int j = 0; j < _power; ++j) {
      if (j == i)
        continue;
      computeRoude(neigb, _power, far_dir);
      int far_neigb = neigb + far_dir[j];
      if (far_neigb == _reciever) {
        res = (i << 2) | 1;
        delete[] far_dir;
        return res;
      }
    }
  }
  delete[] far_dir;
  return res;
}

int findRoude(const int rank_my, const int rank_past, const int power) {
  int* coll = new int[power];
  computeRoude(rank_past, power, coll);
  int roude = 0;
  for (int i = 0; i < power; ++i) {
    if (rank_past + coll[i] == rank_my) {
      roude = (i + 1) % power;
      break;
    }
  }
  return roude;
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

int hyperTopoRecv(const int rank, const int size, double* collect) {
  int mysize = 0, myrank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &mysize);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  bool isCorr = checkProc(mysize);
  MPI_Status stat, stat1;
  MPI_Request* reqs;
  if (rank == myrank && isCorr) {
    int* roud = new int[computeSize(mysize)];
    MPI_Recv(roud, computeSize(mysize), MPI_INT, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, &stat1);
    MPI_Recv(&collect[0], size, MPI_DOUBLE, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, &stat);
    int fin = -1;
    reqs = new MPI_Request[mysize];
    for (int i = 0; i < mysize; ++i) {
      if (i == myrank || i == stat.MPI_SOURCE ||
        (((roud[i / (sizeof(int) * 8) + 1]) & (1 << (i % (sizeof(double) * 8)))) >> (i % (sizeof(double) * 8))) == 1) {
        continue;
      }
      MPI_Isend(&fin, 1, MPI_INT, i, rank, MPI_COMM_WORLD, &reqs[i]);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return 0;
}

bool huperCubeTopology(std::vector<double> _message, const int _receiver, const int _sender) {
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Barrier(MPI_COMM_WORLD);

  bool isCorr = checkProc(size);
  if (_sender >= size || _receiver >= size || !isCorr) {
    return 0;
  }

  if (_receiver == _sender ||
      size == 1) {
    int* count = new int[computeSize(size)]{};
    count[0] = _message.size();
    count[rank / (sizeof(int) * 8) + 1] |= 1 << (rank % (sizeof(double) * 8));
    double* arr = new double[_message.size()];
    for (int i = 0; i < static_cast<int>(_message.size()); ++i)
      arr[i] = _message[i];
    MPI_Isend(count, computeSize(size), MPI_INT, _receiver, _receiver, MPI_COMM_WORLD, &globreq);
    MPI_Isend(arr, _message.size(), MPI_DOUBLE, _receiver, _receiver, MPI_COMM_WORLD, &globreq);
    return 0;
  }

  if (rank == _receiver) {
    return 0;
  }

  int power = powerHyperCube(size);
  int* comms_offsets = new int[power];
  computeRoude(rank, power, comms_offsets);

  int near_path;

  if (rank == _sender) {
    int* count = new int[computeSize(size)]{};
    count[0] = _message.size();
    count[rank / (sizeof(int) * 8) + 1] |= 1 << (rank % (sizeof(double) * 8));
    near_path = reciver_nearby(rank, _receiver, power, comms_offsets);

    if (near_path) {
      int nearby_to = near_path >> 2;
      MPI_Send(count, computeSize(size), MPI_INT, rank + comms_offsets[nearby_to], _receiver, MPI_COMM_WORLD);
      MPI_Send(&_message[0], _message.size(), MPI_DOUBLE, rank + comms_offsets[nearby_to], _receiver, MPI_COMM_WORLD);
    } else {
      MPI_Send(count, computeSize(size), MPI_INT, rank + comms_offsets[0], _receiver, MPI_COMM_WORLD);
      MPI_Send(&_message[0], _message.size(), MPI_DOUBLE, rank + comms_offsets[0], _receiver, MPI_COMM_WORLD);
    }
  } else {
    int to = 0;
    int *count = new int[computeSize(size)];
    MPI_Status stat;
    MPI_Recv(count, computeSize(size), MPI_INT, MPI_ANY_SOURCE, _receiver, MPI_COMM_WORLD, &stat);
    count[rank / (sizeof(int) * 8) + 1] |= 1 << (rank % (sizeof(double) * 8));
    if (count[0] != -1) {
      double* mess = new double[count[0]];
      near_path = reciver_nearby(rank, _receiver, power, comms_offsets);
      int nearby_to = near_path >> 2;
      MPI_Recv(&mess[0], count[0], MPI_DOUBLE, MPI_ANY_SOURCE, _receiver, MPI_COMM_WORLD, &stat);

      if (near_path == 0) {
        to = findRoude(rank, stat.MPI_SOURCE, power);
        MPI_Send(count, computeSize(size), MPI_INT, rank + comms_offsets[to], _receiver, MPI_COMM_WORLD);
        MPI_Send(&mess[0], count[0], MPI_DOUBLE, rank + comms_offsets[to], _receiver, MPI_COMM_WORLD);
      } else {
        MPI_Send(count, computeSize(size), MPI_INT, rank + comms_offsets[nearby_to], _receiver, MPI_COMM_WORLD);
        MPI_Send(&mess[0], count[0], MPI_DOUBLE, rank + comms_offsets[nearby_to], _receiver, MPI_COMM_WORLD);
      }
    }
  }

  return 0;
}
