// Copyright 2020 Sozinov Alex
#ifndef MODULES_TASK_2_SOZINOV_A_GAUSS_HORISONTAL_GAUSS_HORISONTAL_H_
#define MODULES_TASK_2_SOZINOV_A_GAUSS_HORISONTAL_GAUSS_HORISONTAL_H_

#include <vector>

std::vector<double> GetSequentialSolution(const std::vector<double>& _coefs,
                       const std::vector<double>& _rightPart, int size);
std::vector<double> GetParalSolution(const std::vector<double>& _coefs,
                       const std::vector<double>& _rightPart, int size);
int FindPivotRows(const std::vector<double>& coefs, const std::vector<int>& rowOrder, int size, int curEquat);

#endif  // MODULES_TASK_2_SOZINOV_A_GAUSS_HORISONTAL_GAUSS_HORISONTAL_H_
