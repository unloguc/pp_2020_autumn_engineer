// Copyright 2020  Kren Polina
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../modules/task_1/kren_p_find_max_element_of_matrix_by_rows/find_max_element_of_matrix_by_rows.h"


std::vector<int> getRandomMatrix(int rows, int columns) {
    if (rows <= 0 || columns <= 0)
        throw "Incorrect data";
    int size = rows * columns;
    std::vector<int> matrix(size);
    std::mt19937 random_gen;
    random_gen.seed(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < size; i++)
        matrix[i] = random_gen() % 100;
    return matrix;
}

std::vector<int> getMaxInRows(const std::vector<int>& matrix, int rows, int columns) {
    if (rows <= 0 || columns <= 0)
        throw "Incorrect data";
    std::vector<int> vector_of_max_elements(rows);
    for (int i = 0; i < rows; i++) {
        vector_of_max_elements[i] = *std::max_element(matrix.begin() + i * columns,
            matrix.begin() + i * columns + columns);
    }
    return vector_of_max_elements;
}

std::vector<int> getMaxInRowsParall(const std::vector<int>& main_matrix, int rows, int columns) {
    int main_size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &main_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> main_result(rows);

    int delta = rows / main_size;
    std::vector<int> result(delta);
    std::vector<int> matrix(delta * columns);

    MPI_Scatter(&main_matrix[0], delta * columns, MPI_INT, &matrix[0], delta * columns, MPI_INT, 0, MPI_COMM_WORLD);
    result = getMaxInRows(matrix, delta, columns);
    MPI_Gather(&result[0], delta, MPI_INT, &main_result[0], delta, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        if (rows % main_size != 0) {
            for (int i = delta * main_size; i < rows; i++) {
                main_result[i] = main_matrix[i * columns];
                for (int j = 1; j < columns; j++) {
                    if (main_matrix[i * columns + j] > main_result[i]) {
                        main_result[i] = main_matrix[i * columns + j];
                    }
                }
            }
        }
    }
    return main_result;
}
