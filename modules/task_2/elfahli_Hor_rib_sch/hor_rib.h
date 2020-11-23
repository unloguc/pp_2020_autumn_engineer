// Copyright 2020 MERYEM ELFAHLI

# ifndef MODULES_TASK_2_ELFAHLI_HOR_RIB_SCH_HOR_RIB_H_
# define  MODULES_TASK_2_ELFAHLI_HOR_RIB_SCH_HOR_RIB_H_
#include < vector >
using namespace std;

vector<double> gen_mat(int l, int c);
vector<double> gen_vec(int n);
vector < double > seq(vector < double > M, vector < double > V, int l, int c);
vector < double > par (vector < double > M, vector < double > V, int l, int c);
double  r ( int i,vector < double > M, vector < double > V, int l);

# endif   // MODULES_TASK_2_ELFAHLI_HOR_RIB_SCH_HOR_RIB_H_
