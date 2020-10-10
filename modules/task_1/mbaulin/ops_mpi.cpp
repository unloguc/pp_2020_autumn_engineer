#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../../modules/test_tasks/test_mpi/ops_mpi.h"

int** fillMatrixWithRandomNumbers(int** mat, int rows, int cols)
{
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat[i][j] = gen() % 10000;
        }
    }

    return mat;
}

int findMax(int** mat, int rows, int cols)
{
    int maxElement = INT_MIN;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (mat[i][j] > maxElement) {
                maxElement = mat[i][j];
            }
        }
    }

    return maxElement;
}


int getParallelOperations(int** mat, int rows, int cols)
{   
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int delta = rows / size;

    if (rank == 0) {
        for (int proc = 1; proc < size; proc++) {
            for (int i = proc * delta; (proc + 1) * delta ; i++)
            {
                if (i > rows) break;
                MPI_Send(mat[i], cols,
                    MPI_INT, proc, 0, MPI_COMM_WORLD);
            }
        }
    }

    int** local_mat = new int* [delta];
    for (int i = 0; i < delta; i++)
        local_mat[i] = new int[cols];

    if (rank == 0) {
        local_mat = mat;
    }
    else {
        for (int i = rank * delta; (rank + 1) * delta; i++)
        {
            MPI_Status status;
            MPI_Recv(local_mat[i], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
        MPI_Status status;
    }

    int global_sum = 0;
    int local_sum = findMax(local_mat, delta, cols);
    MPI_Op op_code = MPI_MAX;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return global_sum;
}