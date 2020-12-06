// Copyright 2020 Eremina Alena
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include "../../../modules/task_3/eremina_a_cannon/cannon.h"
#include <stdio.h>

std::vector<double> getRandomMatrix(int size) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<double> matrix(size*size);
    for (int i = 0; i < size * size; i++) { 
        matrix[i] = (gen() % 1000) / 7;
    }
    return matrix;
}

void CartesianComm(int block_num, MPI_Comm* BlockComm) {
    int ProcNum;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

    int dims[2] = { block_num, block_num };  // массив длины 2, определяющий количество процессов в каждой размерности решётки
    int periods[2] = { 1, 1 }; // периодичность
    int reorder = 1;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, BlockComm); // создание решётки
}

std::vector<double> getSequentialMultiply(std::vector<double> a, std::vector<double> b, int size) {
    std::vector<double> c(size*size);
    double sum;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            sum = 0.0;
            for (int k = 0; k < size; k++) {
                sum += a[i * size + k] * b[k * size + j];
            }
            c[i * size + j] = sum;
        }
    }
    return c;
}

std::vector<double> getParallelMultiply(std::vector<double> a, std::vector<double> b, int size) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    std::vector<double> c(size*size, 0.0);

    int BlockNum, BlockSize;
    int bCastData[2];
    if (ProcRank == 0) {
        BlockNum = (int)sqrt(ProcNum);
        if (BlockNum * BlockNum != ProcNum) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        BlockSize = size / BlockNum;
        bCastData[0] = BlockNum;
        bCastData[1] = BlockSize;
    }

    MPI_Bcast(&bCastData, 2, MPI_INT, 0, MPI_COMM_WORLD);
    BlockNum = bCastData[0];
    BlockSize = bCastData[1];
	 
	if (size % BlockNum != 0) {
		throw -1;
		MPI_Abort(MPI_COMM_WORLD, 2);
	}

    // Создание декартовой решётки процессов (2D)
    MPI_Comm BlockComm;
    CartesianComm(BlockNum, &BlockComm);

    // Блоки
    std::vector<double> blockA(BlockSize*BlockSize);
    std::vector<double> blockB(BlockSize*BlockSize);

    // Создание типа данных для описания подмассивов глобального массива
    int globalSize[2] = { size, size };
    int localSize[2] = { BlockSize, BlockSize };
    int starts[2] = { 0,0 };
    MPI_Datatype type, subarrType;
    MPI_Type_create_subarray(2, globalSize, localSize, starts, MPI_ORDER_C, MPI_DOUBLE, &type);
    MPI_Type_create_resized(type, 0, BlockSize * sizeof(double), &subarrType);
    MPI_Type_commit(&subarrType);

    // Размещаем массивы по процессам
    int* sendcounts = (int*)malloc(sizeof(int) * ProcNum);
    int* displs = (int*)malloc(sizeof(int) * ProcNum);
    if (ProcRank == 0) {
        for (int i = 0; i < ProcNum; i++) {
            sendcounts[i] = 1;
        }
        int disp = 0;
        for (int i = 0; i < BlockNum; i++) {
            for (int j = 0; j < BlockNum; j++) {
                displs[i * BlockNum + j] = disp;
                disp += 1;
            }
            disp += (BlockSize - 1) * BlockNum;
        }
    }
    MPI_Scatterv(&(a[0]), sendcounts, displs, subarrType, &(blockA[0]), size*size / (ProcNum), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(&(b[0]), sendcounts, displs, subarrType, &(blockB[0]), size*size / (ProcNum), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Начальный сдвиг
    int left, right, up, down;
    int coords[2];
    MPI_Cart_coords(BlockComm, ProcRank, 2, coords);
    MPI_Cart_shift(BlockComm, 1, coords[0], &left, &right);
    MPI_Sendrecv_replace(&(blockA[0]), BlockSize * BlockSize, MPI_DOUBLE, left, 1, right, 1, BlockComm, MPI_STATUS_IGNORE);
    MPI_Cart_shift(BlockComm, 0, coords[1], &up, &down);
    MPI_Sendrecv_replace(&(blockB[0]), BlockSize * BlockSize, MPI_DOUBLE, up, 1, down, 1, BlockComm, MPI_STATUS_IGNORE);

    // Результирующий блок
    std::vector<double> blockC(BlockSize*BlockSize, 0.0);

    // Блок для подсчёта
    std::vector<double> blockRes(BlockSize*BlockSize);

    for (int k = 0; k < BlockNum; k++) {
        blockRes = getSequentialMultiply(blockA, blockB, BlockSize);

        for (int i = 0; i < BlockSize * BlockSize; i++) {
            blockC[i] += blockRes[i];
        }

        // Сдвигаем A влево и B вверх (на единицу)
        MPI_Cart_shift(BlockComm, 1, 1, &left, &right);
        MPI_Cart_shift(BlockComm, 0, 1, &up, &down);
        MPI_Sendrecv_replace(&(blockA[0]), BlockSize * BlockSize, MPI_DOUBLE, left, 1, right, 1, BlockComm, MPI_STATUS_IGNORE);
        MPI_Sendrecv_replace(&(blockB[0]), BlockSize * BlockSize, MPI_DOUBLE, up, 1, down, 1, BlockComm, MPI_STATUS_IGNORE);
    }

    // Результат
    MPI_Gatherv(&(blockC[0]), size * size / ProcNum, MPI_DOUBLE, &(c[0]), sendcounts, displs, subarrType, 0, MPI_COMM_WORLD);

    return c;
}
