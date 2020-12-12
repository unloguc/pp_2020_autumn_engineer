// Copyright 2020 Chesnokov Artyom
#include "../../../modules/task_1/chesnokov_a_matrix_column_sum/matrix_column_sum.h"
#include <mpi.h>
#include <random>
#include <ctime>
#include <vector>
#include <algorithm>

#include <iostream>

#define MPI_CALL(x)\
if (x != MPI_SUCCESS) { \
    std::cout << #x << __FILE__ << __LINE__ << std::endl; \
} \

Matrix getRandomMatrix(int columns, int rows) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    Matrix matrix(columns, rows);
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            matrix.buf[i * rows + j] = gen() % 128;
        }
    }
    return matrix;
}

std::vector<int> getSequentialColumnSum(const Matrix & matrix) {
    std::vector<int> res(matrix.columns);
    for (size_t i = 0; i < res.size(); i++) {
        res[i] = 0;
    }
    for (int i = 0; i < matrix.columns; i++) {
        for (int j = 0; j < matrix.rows; j++) {
            res[i] += matrix.buf[i * matrix.rows + j];
        }
    }
    return res;
}

std::vector<int> getParallelColumnSum(const Matrix & matrix) {
    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (matrix.columns < size) {
        return getSequentialColumnSum(matrix);
    }

    // allocating memory for each process
    // last one gets more then others if columns % size != 0
    int delta = matrix.columns / size;
    int rem = delta + matrix.columns % size;
    Matrix * local_submatrix;
    if (rank == 0) {
      local_submatrix = new Matrix(rem, matrix.rows, matrix.buf);
    } else {
        local_submatrix = new Matrix(delta, matrix.rows);
    }

    if (rank == 0) {
      for (int proc = 1; proc < size; proc++) {
          MPI_Send(&matrix.buf[rem * matrix.rows + delta * (proc - 1) * matrix.rows],
              matrix.rows * delta, MPI_INT, proc, 0, MPI_COMM_WORLD);
      }
    } else {
        MPI_Status status;
        MPI_Recv(local_submatrix->buf, matrix.rows * delta,
            MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    // sequently counting column sum
    std::vector<int> sub_vec = getSequentialColumnSum(*local_submatrix);

    std::vector<int> res(matrix.columns);
    if (rank == 0) {
      std::copy(sub_vec.begin(), sub_vec.end(), res.begin());
      MPI_Status status;
      for (int i = 0; i < size - 1; i++) {
          MPI_Recv(&res[0] + rem + i * delta, delta, MPI_INT,
              i + 1, 0, MPI_COMM_WORLD, &status);
      }
    } else {
          MPI_Send(&sub_vec[0], sub_vec.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    delete local_submatrix;
    return res;
}

Matrix::Matrix(int col, int r) : columns(col), rows(r) {
    buf = new int[col * r];
    for (int i = 0; i < col * r; i++) {
        buf[i] = 0;
    }
}

Matrix::Matrix(int col, int r, int * buf) : columns(col), rows(r) {
    this->buf = new int[col * r];
    for (int i = 0; i < col * r; i++) {
        this->buf[i] = buf[i];
    }
}

Matrix::Matrix(const Matrix & m) : columns(m.columns), rows(m.rows) {
    this->buf = new int[columns * rows];
    for (int i = 0; i < columns * rows; i++) {
        this->buf[i] = m.buf[i];
    }
}
