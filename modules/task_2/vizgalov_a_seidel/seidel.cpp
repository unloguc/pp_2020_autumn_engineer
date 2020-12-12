// Copyright 2020 Vizgalov Anton
#include "../../../modules/task_2/vizgalov_a_seidel/seidel.h"


double calculatePrecision(int size, double* res, double* prevRes) {
    double maxDiff = 0;
    for (int i = 0; i < size; ++i) {
        double diff = fabs(res[i] - prevRes[i]);

        if (diff > maxDiff) {
            maxDiff = diff;
        }
    }

    double e = 1;
    while (maxDiff < e) {
        e /= 10;
    }
    return e * 10;
}

void seidelSequential(int size, double** mat, double* vec, int numIterations, double* res, double* precision) {
    double* x = new double[size];
    double* prevX = new double[size];
    for (int i = 0; i < size; ++i) {
        x[i] = 0;
        prevX[i] = 0;
    }

    for (int i = 0; i < numIterations; ++i) {
        for (int j = 0; j < size; ++j) {
            double sum = 0;
            for (int k = 0; k < size; ++k) {
                sum += mat[j][k] * x[k];
            }
            sum -= mat[j][j] * x[j];

            prevX[j] = x[j];
            x[j] = (vec[j] - sum) / mat[j][j];
        }
    }

    *precision = calculatePrecision(size, x, prevX);
    for (int i = 0; i < size; ++i) {
        res[i] = x[i];
    }

    delete[] x;
    delete[] prevX;
}

void seidelParallel(int size, double** mat, double* vec, int numIterations, double* res, double* precision) {
    MPI_Status status;
    int commSize, procRank;
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    if (commSize < 2) {
        seidelSequential(size, mat, vec, numIterations, res, precision);
        return;
    }

    const int numPerProc = size / commSize;
    const int leftover = size % commSize;
    const int numOnLastProc = numPerProc + leftover;

    double* x = new double[size];
    double* prevX = new double[size];
    for (int i = 0; i < size; ++i) {
        x[i] = 0;
        prevX[i] = 0;
    }

    double* localVec;
    double* globalVec;
    if (procRank + 1 != commSize) {
        localVec = new double[numPerProc];
        globalVec = new double[numPerProc * size];
    } else {
        localVec = new double[numOnLastProc];
        globalVec = new double[numOnLastProc * size];
    }

    MPI_Scatter(&vec[0], numPerProc, MPI_DOUBLE, &localVec[0], numPerProc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (procRank == 0) {
        for (int i = 1; i < commSize; ++i) {
            for (int j = 0; j < numPerProc; ++j) {
                MPI_Send(&mat[i * numPerProc + j][0], size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            }
        }

        if (leftover != 0) {
            MPI_Send(&vec[commSize * numPerProc], leftover, MPI_DOUBLE, (commSize - 1), 0, MPI_COMM_WORLD);

            for (int i = 0; i < leftover; ++i) {
                MPI_Send(&mat[commSize * numPerProc + i][0], size, MPI_DOUBLE, (commSize - 1), 0, MPI_COMM_WORLD);
            }
        }
    } else if (procRank + 1 == commSize) {
        for (int i = 0; i < numPerProc; ++i) {
            MPI_Recv(&globalVec[size * i], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        }

        if (leftover != 0) {
            MPI_Recv(&localVec[numPerProc], leftover, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

            for (int i = 0; i < leftover; ++i) {
                MPI_Recv(&globalVec[(numPerProc + i) * size], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
            }
        }
    } else {
        for (int i = 0; i < numPerProc; ++i) {
            MPI_Recv(&globalVec[size * i], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        }
    }

    for (int iter = 0; iter < numIterations; ++iter) {
        if (procRank == 0) {
            for (int i = 0; i < numPerProc; ++i) {
                double sum = 0;
                for (int j = 0; j < i; ++j) {
                    sum += mat[i][j] * x[j];
                }
                for (int j = i + 1; j < size; ++j) {
                    sum += mat[i][j] * prevX[j];
                }
                x[i] = (localVec[i] - sum) / mat[i][i];

                for (int j = 1; j < commSize; ++j) {
                    MPI_Send(&x[i], 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD);
                }
            }
        } else if (procRank + 1 == commSize) {
            for (int i = 0; i < procRank; ++i) {
                for (int j = 0; j < numPerProc; ++j) {
                    MPI_Recv(&x[i * numPerProc + j], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
                }
            }

            for (int i = 0; i < numOnLastProc; ++i) {
                int midIdx = numPerProc * procRank + i;
                double sum = 0;
                for (int j = 0; j < midIdx; ++j) {
                    sum += globalVec[i * size + j] * x[j];
                }
                for (int j = midIdx + 1; j < size; ++j) {
                    sum += globalVec[i * size + j] * prevX[j];
                }
                x[midIdx] = (localVec[i] - sum) / globalVec[i * size + midIdx];
            }
        } else {
            for (int i = 0; i < procRank; ++i) {
                for (int j = 0; j < numPerProc; ++j) {
                    MPI_Recv(&x[i * numPerProc + j], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
                }
            }

            for (int i = 0; i < numPerProc; ++i) {
                int midIdx = numPerProc * procRank + i;
                double sum = 0;
                for (int j = 0; j < midIdx; ++j) {
                    sum += globalVec[i * size + j] * x[j];
                }
                for (int j = midIdx + 1; j < size; ++j) {
                    sum += globalVec[i * size + j] * prevX[j];
                }
                x[midIdx] = (localVec[i] - sum) / globalVec[i * size + midIdx];

                for (int j = procRank + 1; j < commSize; ++j) {
                    MPI_Send(&x[midIdx], 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD);
                }
            }
        }

        if (procRank == 0) {
            MPI_Recv(&x[0], size, MPI_DOUBLE, (commSize - 1), 0, MPI_COMM_WORLD, &status);

            *precision = calculatePrecision(size, x, prevX);
        } else if (procRank + 1 == commSize) {
            MPI_Send(&x[0], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < size; ++i) {
            prevX[i] = x[i];
        }

        MPI_Bcast(&prevX[0], size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    for (int i = 0; i < size; ++i) {
        res[i] = x[i];
    }

    delete[] x;
    delete[] prevX;
    delete[] localVec;
    delete[] globalVec;
}
