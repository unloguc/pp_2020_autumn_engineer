// Copyright 2018 Nesterov Alexander
#ifndef MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_
#define MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_

#include <vector>
#include <string>

std::string getRandomString(int sz);
int countFrequencyPar(const std::string& global_string, char symbol, int size);
int countFrequencySec(const std::string& m_string, char symbol, int size);

#endif  // MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_
