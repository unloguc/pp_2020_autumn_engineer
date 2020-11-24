// Copyright 2020 MERYEM EL FAHLI
#ifndef MODULES_TASK_2_ELFAHLI_HOR_RIB_SCH_HOR_RIB_H_
#define MODULES_TASK_2_ELFAHLI_HOR_RIB_SCH_HOR_RIB_H_
#include <vector>


std::vector<double> gen_mat(int l, int c);
std::vector<double> gen_vec(int n);
std::vector < double > seq(std::vector < double > M, std::vector < double > V, int l, int c);
std::vector < double > par(std::vector < double > M, std::vector < double > V, int l, int c);
double  r(int i, std::vector < double > M, std::vector < double > V, int l);

#endif   // MODULES_TASK_2_ELFAHLI_HOR_RIB_SCH_HOR_RIB_H_
