// Copyright 2020 Ndah Rony
#ifndef MODULES_TASK_2_NDAH_R_INTERATIVE_METHOD_INTERATIVE_METHOD_H_
#define MODULES_TASK_2_NDAH_R_INTERATIVE_METHOD_INTERATIVE_METHOD_H_

#include <mpi.h>
#include <cstdlib>
#include <vector>
#include <random>
using std::vector;
double fct(const std::vector<double>& x);

std::vector<double> Seq(const vector<vector<double> >& A, const vector<double>& b, int n, double eps);
std::vector<double> Par(const vector<vector<double> >& A, const vector<double>& _b, int _n, double eps);

#endif  // MODULES_TASK_2_NDAH_R_INTERATIVE_METHOD_INTERATIVE_METHOD_H_
