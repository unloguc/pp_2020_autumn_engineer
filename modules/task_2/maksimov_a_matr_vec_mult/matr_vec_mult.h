// Copyright 2020 Maksimov Andrey
#ifndef MODULES_TASK_2_MAKSIMOV_A_MATR_VEC_MULT_MATR_VEC_MULT_H_
#define MODULES_TASK_2_MAKSIMOV_A_MATR_VEC_MULT_MATR_VEC_MULT_H_

int* getRandomVector(int size, int seedMod = 0);
int** getRandomMatrix(int rows, int columns);
int* multiplyMatrixByVectorNotParall(
    int** matr, int rows, int columns,
    int* vec, int vecSize);
int* multiplyMatrixByVector(
    int** matr, int rows, int columns,
    int* vec, int vecSize);

#endif  // MODULES_TASK_2_MAKSIMOV_A_MATR_VEC_MULT_MATR_VEC_MULT_H_
