// Copyright 2020 Sozinov Alex
#ifndef MODULES_TASKS_1_MAX_RAW_MATRIX_H
#define MODULES_TASKS_1_MAX_RAW_MATRIX_H

#include <vector>
#include <random>
#include <mpi.h>

std::vector<int> GenerateMatrix(int sizeRow, int countRow, int min, int max);
std::vector<int> GetSequentialMax(std::vector<int> &vect, int sizeRow, int countRow);
std::vector<int> GetParalMax(std::vector<int> &vect, int sizeRow, int countRow);

#endif //MODULES_TASKS_1_MAX_RAW_MATRIX_H
