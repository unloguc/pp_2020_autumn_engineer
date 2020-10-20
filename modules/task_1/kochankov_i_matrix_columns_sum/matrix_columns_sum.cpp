// Copyright 2020 Kochankov Ilya
#include <mpi.h>
#include <random>
#include <ctime>
#include <vector>
#include <algorithm>
#include "../../../modules/task_1/kochankov_i_matrix_columns_sum/matrix_columns_sum.h"


vector<double> sequential_operations(const vector<vector<double>>& matrix, int result_size, int start_index) {
    vector<double> result(result_size);
    std::fill(result.begin(), result.end(), 0);

    for (int j = start_index; j < start_index + result_size; j++) {
        for (auto &i : matrix) {
            result[j-start_index] += i[j];
        }
    }

    return result;
}

vector<double> parallel_sum(const vector<vector<double >>& matrix) {
    int size, rank, len = matrix[0].size();

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    vector<double> columns_sums(len);

    const int delta = len / size;
    int start_index = delta * rank;

    int result_size;

    if (delta * (rank + 1) > len) {
        result_size = len - start_index;
    } else {
        result_size = delta;
    }

    auto tmp_result_vector = sequential_operations(matrix, result_size, start_index);

    MPI_Gather(&tmp_result_vector[0], result_size, MPI_DOUBLE,
            &columns_sums[0]+(rank*delta), result_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    return columns_sums;
}

vector<vector<double >> get_rand_matrix(int x, int y) {
    std::mt19937 gen;

    gen.seed(static_cast<unsigned int>(time(0)));
    vector<vector<double >> vec(x);
    for (int i = 0; i < x; i++) {
        vec[i].resize(y);
        for (int j = 0; j < y; j++) {
            vec[i][j] = (static_cast<double>(gen()) / 100000);
        }
    }
    return vec;
}
