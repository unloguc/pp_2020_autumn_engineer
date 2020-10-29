// Copyright 2020 Sandalov Konstantin
#ifndef MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_
#define MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_

#include <string>

std::string getRandomString(int string_size);
int countFrequencyPar(const std::string& global_string, char symbol, int string_size);
int countFrequencySec(const std::string& current_string, char symbol, int string_size);

#endif  // MODULES_TEST_TASKS_TEST_MPI_OPS_MPI_H_
