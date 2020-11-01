// Copyright 2018 Nesterov Alexander
#include <mpi.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include "matrix_max_mpi.h"


std::vector<int> create_random_matrix(const size_t rows, const size_t columns) {
    size_t size = rows * columns;
    std::mt19937 gen(uint32_t(time(0)));
    std::uniform_int_distribution<int> uid(-50, +50);
    std::vector<int> matrix(size);

    for (size_t i = 0; i < size; i++) {
        matrix[i] = uid(gen);
    }
    return matrix;
}

std::vector<int> get_max_elements_of_rows(std::vector<int> matrix, const size_t rows, const size_t columns) {
    std::vector<int> max_elements(rows);
    for (size_t row = 0; row < rows; row++) {
        int max_value = INT_MIN;
        for (size_t column = 0; column < columns; column++) {
            if (max_value < matrix[row * columns + column]) {
                max_value = matrix[row * columns + column];
            }
        }
        max_elements[row] = max_value;
    }
    return max_elements;
}

void print_matrix(std::vector<int> matrix, const size_t rows, const size_t columns, std::string prefix) {
    std::cout << "\n" << prefix << "Matrix: {\n";
    for (size_t row = 0; row < rows; row++) {
        std::cout << prefix << "  ";
        for (size_t column = 0; column < columns; column++) {
            std::cout << matrix[row * columns + column] << ", ";
        }
        std::cout << "\n";
    }
    std::cout << prefix << "}\n";
}

std::vector<int> get_max_elements_of_rows_sequentional(std::vector<int> matrix, const size_t rows, const size_t columns) {
    return get_max_elements_of_rows(matrix, rows, columns);
}

std::vector<int> get_max_elements_of_rows_parallel(const std::vector<int>& matrix, const size_t rows, const size_t columns) {
    std::vector<int> max_elements(rows);

    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    printf("[%d] Process: %d of %d, Rows: %d, Columns: %d\n", ProcRank, ProcRank, ProcNum, rows, columns);

    size_t count_rows_per_process = rows / ProcNum;
    size_t count_extra_rows = rows % ProcNum;

    printf("[%d] Rows per process: %d,  extra rows: %d\n", ProcRank, count_rows_per_process, count_extra_rows);

    if (ProcRank == 0) {
        print_matrix(matrix, rows, columns, "[" + std::to_string(ProcRank) + "] initial  ");
    }

    std::vector<int> sent_matrix_rows(count_rows_per_process * columns);

    MPI_Scatter(matrix.data(), count_rows_per_process * columns, MPI_INT,
                sent_matrix_rows.data(), count_rows_per_process * columns, MPI_INT, 0, MPI_COMM_WORLD);

    printf("[%d] Rows per process: %d,  extra rows: %d\n", ProcRank, count_rows_per_process, count_extra_rows);
    print_matrix(sent_matrix_rows, count_rows_per_process, columns, "[" + std::to_string(ProcRank) + "] sent_matrix_raws  ");

    std::vector<int> receive_elements_max = get_max_elements_of_rows(sent_matrix_rows, count_rows_per_process, columns);
    print_matrix(receive_elements_max, count_rows_per_process, 1, "[" + std::to_string(ProcRank) + "] receive_elements_max  ");

    std::vector<int> max_values;
    if (ProcRank == 0) {
        max_values = std::vector<int>(rows);
    }

    MPI_Gather(receive_elements_max.data(), count_rows_per_process, MPI_INT,
               max_values.data(), count_rows_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    if (ProcRank == 0) {
        if (0 && count_extra_rows) {
            int count_base_rows = count_rows_per_process * ProcNum;
            std::vector<int> extra_elements_max = get_max_elements_of_rows(
                std::vector<int>(&matrix[count_base_rows * columns],
                                 &matrix[(count_base_rows + count_base_rows) * columns]), count_extra_rows, columns);
            print_matrix(extra_elements_max, count_extra_rows, 1, "[" + std::to_string(ProcRank) + "] extra_elements_max  ");
            for (size_t extra_row = 0; extra_row < count_extra_rows; extra_row++) {
                max_values[count_base_rows + extra_row] = extra_elements_max[extra_row];
            }
        }
        print_matrix(max_values, rows, 1, "[" + std::to_string(ProcRank) + "] max_values  ");
    }

    return max_values;
}

int getSequentialOperations(std::vector<int> vec, std::string ops) {
    const int  sz = vec.size();
    int reduction_elem = 0;
    if (ops == "+") {
        for (int  i = 0; i < sz; i++) {
            reduction_elem += vec[i];
        }
    } else if (ops == "-") {
        for (int  i = 0; i < sz; i++) {
            reduction_elem -= vec[i];
        }
    } else if (ops == "max") {
        reduction_elem = vec[0];
        for (int  i = 1; i < sz; i++) {
            reduction_elem = std::max(reduction_elem, vec[i]);
        }
    }
    return reduction_elem;
}

int getParallelOperations(std::vector<int> global_vec,
                          int count_size_vector, std::string ops) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int delta = count_size_vector / size;

    if (rank == 0) {
        for (int proc = 1; proc < size; proc++) {
            MPI_Send(&global_vec[0] + proc * delta, delta,
                        MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    }

    std::vector<int> local_vec(delta);
    if (rank == 0) {
        local_vec = std::vector<int>(global_vec.begin(),
                                     global_vec.begin() + delta);
    } else {
        MPI_Status status;
        MPI_Recv(&local_vec[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int global_sum = 0;
    int local_sum = getSequentialOperations(local_vec, ops);
    MPI_Op op_code;
    if (ops == "+" || ops == "-") { op_code = MPI_SUM; }
    if (ops == "max") { op_code = MPI_MAX; }
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return global_sum;
}
