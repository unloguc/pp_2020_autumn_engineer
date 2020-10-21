// Copyright 2020 Kochankov Ilya
#include <mpi.h>
#include <random>
#include <ctime>
#include <vector>
#include <algorithm>
#include "../../../modules/task_1/kochankov_i_matrix_columns_sum/matrix_columns_sum.h"


vector<int> sequential_operations(const vector<vector<int>>& matrix) {
    vector<int> result(matrix[0].size());
    std::fill(result.begin(), result.end(), 0);

    for (int i = 0; i < static_cast<int>(matrix.size()); i++) {
        for (int j = 0; j < static_cast<int>(matrix[i].size()); j++) {
            result[j] += matrix[i][j];
        }
    }

    return result;
}

vector<int> parallel_sum(const vector<vector<int>>& matrix) {
    int size, rank, len = static_cast<int>(matrix.size());

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    vector<int> columns_sums(len);

    if (len < size) {
        if (rank == 0) {
            return sequential_operations(matrix);
        } else {
            return vector<int>();
        }
    }

    int delta = len / size;
    int start_index;
    vector<vector<int>> local_matrix;

    if (rank == 0) {
        local_matrix = vector<vector<int>>(delta);
        std::copy(matrix.begin(), matrix.begin()+delta, local_matrix.begin());

        int count = delta;
        for (int process_num = 1; process_num < size; process_num++) {
            start_index = delta * process_num;
            if (process_num == size - 1) {
                count = len - start_index;
            }
            for (int vector_num = start_index; vector_num < start_index+count; vector_num++) {
                MPI_Send(&matrix[vector_num][0], matrix[vector_num].size(), MPI_INT, process_num, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        if (rank == size - 1) {
            delta = len - delta * (size - 1);
        }

        local_matrix = vector<vector<int>>(delta);
        for (int vector_num = 0; vector_num < delta; vector_num++) {
            local_matrix[vector_num] = vector<int>(matrix[0].size());
            MPI_Recv(&local_matrix[vector_num][0], matrix[0].size(), MPI_INT, 0, 0, MPI_COMM_WORLD, nullptr);
        }
    }
    auto local_result = sequential_operations(local_matrix);

    vector<int> global_result(matrix[0].size());
    MPI_Reduce(&local_result[0], &global_result[0],
            global_result.size(), MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    return global_result;
}

vector<vector<int >> get_rand_matrix(int x, int y) {
    std::mt19937 gen;

    gen.seed(static_cast<unsigned int>(time(0)));
    vector<vector<int >> vec(x);
    for (int i = 0; i < x; i++) {
        vec[i].resize(y);
        for (int j = 0; j < y; j++) {
            vec[i][j] = (static_cast<double >(gen()) / 100000);
        }
    }
    return vec;
}
