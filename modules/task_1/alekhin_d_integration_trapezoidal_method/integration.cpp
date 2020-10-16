// Copyright 2020 Alekhin Denis
#include <mpi.h>
#include<iostream>
#include <vector>
#include "../../modules/task_1/alekhin_d_integration_trapezoidal_method/integration.h"

double myFunc(double x) { return x / 2; }

std::function<double(double)> f = myFunc;

double localIntegration(double func_x[], double func_y[], int size) {
  double local_integral = 0;
  double height = func_x[1] - func_x[0];

  for (int i = 0; i < size - 1; i++)
  local_integral += height * (func_y[i] + func_y[i + 1]) / 2;

  return local_integral;
}

double integration(std::function<double(double)> func, int n, int a, int b) {
  if (n < 1) throw "Error: n < 1";
  if (a > b) throw "Error: a > b";

  int size, rank;

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (n < size) throw "Error: a > b";

  double** func_x = new double* [size];
  double** func_y = new double* [size];

  double step_x = (b - a) / static_cast<double>(n);
  double local_integ_width = (b - a) / static_cast<double>(size);

  if (rank == 0) {
    for (int i = 0; i < size; i++) {
      double* local_integ_x;
      double* local_integ_y;

      if (n % size == 0 || i != size - 1) {
        local_integ_x = new double[n / size];
        local_integ_y = new double[n / size];
        for (int j = 0; j <= n / size; j++) {
          local_integ_x[j] = a + i * step_x * (n / size) + j * step_x;
          local_integ_y[j] = func(local_integ_x[j]);
        }
      } else {
        local_integ_x = new double[n - (size - 1) * (n / size) + 1];
        local_integ_y = new double[n - (size - 1) * (n / size) + 1];

        for (int j = 0; j <= n - (size - 1) * (n / size); j++) {
          local_integ_x[j] = a + i * step_x * (n / size) + j * step_x;
          local_integ_y[j] = func(local_integ_x[j]);
        }
      }
      func_x[i] = local_integ_x;
      func_y[i] = local_integ_y;
    }
  }

  double* local_func_x;
  double* local_func_y;
  if (n % size != 0 && rank == size - 1) {
    local_func_x = new double[n - (size - 1) * (n / size) + 1];
    local_func_y = new double[n - (size - 1) * (n / size) + 1];
  } else {
    local_func_x = new double[n / size + 1];
    local_func_y = new double[n / size + 1];
  }

  if (rank == 0) {
    for (int i = 1; i < size; i++) {
      if (i == size - 1 && n % size != 0) {
        MPI_Send(func_x[i], n - (size - 1) * (n / size) + 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        MPI_Send(func_y[i], n - (size - 1) * (n / size) + 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
      } else {
      MPI_Send(func_x[i], n / size + 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
      MPI_Send(func_y[i], n / size + 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
    }
  }

  local_func_x = func_x[0];
  local_func_y = func_y[0];
  } else {
    if (n % size != 0 && rank == size - 1) {
      MPI_Recv(local_func_x, n - (size - 1) * (n / size) + 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(local_func_y, n - (size - 1) * (n / size) + 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
      MPI_Recv(local_func_x, n / size + 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(local_func_y, n / size + 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }
  double local_integral = 0;
  if (n % size != 0 && rank == size - 1)
    local_integral = localIntegration(local_func_x, local_func_y, n - (size - 1) * (n / size) + 1);
  else
    local_integral = localIntegration(local_func_x, local_func_y, n / size + 1);
  std::cout << std::endl << local_integral << std::endl;
  double global_integral = 0;
  MPI_Reduce(&local_integral, &global_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&global_integral, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  return global_integral;
}
