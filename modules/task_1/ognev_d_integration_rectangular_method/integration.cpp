// Copyright 2020 Ognev Denis
#include <mpi.h>
#include "../../modules/task_1/ognev_d_integration_rectangular_method/integration.h"

double function(double x) {
  return x * x;
}

double integrationSequential(std::function<double(double)> f, double a, double b, int n){
  if (a > b) throw "Left border can't be less than right";
  if (n < 1) throw "Number of segments can't be less than 1";
  if (f == nullptr) throw "Function error";

  double h = (b - a) / static_cast<double>(n);
  double integral = 0;
  /*for (double i = a; i <= b; i += h){
    integral += f(i + h / 2.0);
  }*/
  for (int i = 0; i < n; i++){
	double x = a + h * i;
    integral += f(x + h / 2.0);
  }
  integral *= h;

  return integral;
}

double integrationParallel(std::function<double(double)> f, double a, double b, int n){
  if (a > b) throw "Left border can't be less than right";
  if (n < 1) throw "Number of segments can't be less than 1";
  if (f == nullptr) throw "Function error";

  int proc_num, proc_rank;
  double sum = 0;
  double h = (b - a) / static_cast<double>(n);
  double integral;

  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

  double parts = (b - a) / static_cast<double>(proc_num);

  /*if (n >= proc_num){
    for (double i = a + proc_rank * parts; i < a + ((static_cast<double>(proc_rank) + 1) * parts); i += h){
      sum += f(i + h / 2.0);
    }
  } else {
    for (double i = a + proc_rank * h; i <= b - (static_cast<double>(n) - (static_cast<double>(proc_rank))) * h; i += h){
      if (proc_rank >= n){
	    break;
	  }
	  sum += f(i + h / 2.0);
	}
  }*/
  /*for (double i = a + proc_rank * h; i < b; i += h * proc_num){
    sum += f(i + h / 2);
  }*/
  for (int i = proc_rank; i < n; i += proc_num){
	double x = a + h * i;
	sum += f(x + h / 2.0);
  }

  sum *= h;

  MPI_Reduce(&sum, &integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&integral, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  return integral;
}

