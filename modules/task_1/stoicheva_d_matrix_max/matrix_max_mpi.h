// Copyright 2018 Nesterov Alexander
#ifndef MODULES_TEST_TASKS_TEST_MPI_MATRIX_MAX_MPI_H_
#define MODULES_TEST_TASKS_TEST_MPI_MATRIX_MAX_MPI_H_

#include <vector>
#include <string>

std::vector<int> getRandomVector(int  sz);
int getParallelOperations(std::vector<int> global_vec,
                          int count_size_vector, std::string ops);
int getSequentialOperations(std::vector<int> vec, std::string ops);

std::vector<int> get_max_elements_of_rows_sequentional(std::vector<int> matrix, const size_t rows, const size_t columns);
std::vector<int> get_max_elements_of_rows_parallel(const std::vector<int>& matrix, const size_t rows, const size_t columns);
std::vector<int> create_random_matrix(size_t rows, size_t columns);
void print_matrix(std::vector<int> matrix, const size_t rows, const size_t columns, std::string prefix);

#endif  // MODULES_TEST_TASKS_TEST_MPI_MATRIX_MAX_MPI_H_
