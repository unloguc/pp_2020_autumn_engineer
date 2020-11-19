// Copyright 2020 Evseev Alexander
#pragma once
#ifndef MODULES_TASK_2_EVSEEV_A_MULT_OF_MATRIX_ONLY_A_MULT_OF_MATRIX_ONLY_A_H_
#define MODULES_TASK_2_EVSEEV_A_MULT_OF_MATRIX_ONLY_A_MULT_OF_MATRIX_ONLY_A_H_

#include <vector>

std::vector<int> random_matrix(int size);
std::vector<int> MatrixMultiplication(const std::vector<int>& A, const std::vector<int>&B, int Size);
std::vector<int>  MatrixMultiplicationMPI(std::vector<int> matrixA, std::vector<int> matrixB, int sizeSide);

#endif  // MODULES_TASK_2_EVSEEV_A_MULT_OF_MATRIX_ONLY_A_MULT_OF_MATRIX_ONLY_A_H_



