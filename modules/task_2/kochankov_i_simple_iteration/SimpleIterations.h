// Copyright 2020 Kochankov Ilya

#ifndef MODULES_TASK_1_KOCHANKOV_I_MATRX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_
#define MODULES_TASK_1_KOCHANKOV_I_MATRX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_

#include "iostream"
#include "vector"

#include "./Matrix.h"

using std::vector;

Matrix get_input_matrix();
double get_input_accuracy();
Matrix get_rand_matrix(int x, int y);
int is_correct_matrix(const Matrix& matrix);
void print_result(const vector<double> &result);
vector<double> linear_simple_iteration(const Matrix& matrix, double accuracy);
vector<double> parallel_simple_iteration(const Matrix& matrix, double accuracy);

#endif // MODULES_TASK_1_KOCHANKOV_I_MATRIX_COLUMNS_SUM_MATRIX_COLUMNS_SUM_H_