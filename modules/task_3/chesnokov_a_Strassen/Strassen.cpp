// Copyright 2020 Chesnokov
#include "../../../modules/task_3/chesnokov_a_Strassen/Strassen.h"
#include <mpi.h>
#include <random>
#include <ctime>
#include <iostream>

using namespace std;

Matrix::Matrix(int col, int r) : columns(col), rows(r) {
    buf = new double[col * r];
    for (int i = 0; i < col * r; i++) {
        buf[i] = 0.0;
    }
}

Matrix::Matrix(int col, int r, double * buf) : columns(col), rows(r) {
    this->buf = new double[col * r];
    for (int i = 0; i < col * r; i++) {
        this->buf[i] = buf[i];
    }
}

Matrix::Matrix(const Matrix & m) : columns(m.columns), rows(m.rows) {
    this->buf = new double[columns * rows];
    for (int i = 0; i < columns * rows; i++) {
        this->buf[i] = m.buf[i];
    }
}

Matrix Matrix::operator+(const Matrix & b) {
    if (columns != b.columns || rows != b.rows)
        throw "Invalid dimension";
    Matrix res(*this);
    for (int i = 0; i < res.columns * res.rows; i++) {
        res.buf[i] += b.buf[i];
    }
    return res;
}

Matrix Matrix::operator-(const Matrix & b) {
    if (columns != b.columns || rows != b.rows)
        throw "Invalid dimension";
    Matrix res(*this);
    for (int i = 0; i < res.columns * res.rows; i++) {
        res.buf[i] -= b.buf[i];
    }
    return res;
}

Matrix & Matrix::operator=(const Matrix & m)
{
    if (buf) delete[] buf;
    columns = m.columns;
    rows = m.rows;
    buf = new double[columns * rows];
    for (int i = 0; i < columns * rows; i++) {
        buf[i] = m.buf[i];
    }
    return *this;
}


Matrix getRandomMatrix(int columns, int rows) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    Matrix matrix(columns, rows);
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            matrix.buf[i * rows + j] = gen() % 10;
        }
    }
    return matrix;
}

Matrix getMatrixMul(const Matrix & a, const Matrix & b) {
    if (a.columns != b.rows) throw "Invalid dimensions";
    Matrix res(a.rows, b.columns);
    for (int i = 0; i < res.rows; i++)
        for (int j = 0; j < res.columns; j++)
            for (int r = 0; r < a.columns; r++) {
                res.buf[i * res.columns + j] +=
                    a.buf[i * a.columns + r] * b.buf[r * b.columns + j];
            }
    return res;
}

// It's supposed that a and b are square matrix, with equal size = 2^n
Matrix getMatrixMulStrassen(const Matrix & a, const Matrix & b) {
    if (a.columns <= 64) return getMatrixMul(a, b);

    Matrix A11(a.columns / 2, a.columns / 2);
    Matrix A12(a.columns / 2, a.columns / 2);
    Matrix A21(a.columns / 2, a.columns / 2);
    Matrix A22(a.columns / 2, a.columns / 2);

    Matrix B11(a.columns / 2, a.columns / 2);
    Matrix B12(a.columns / 2, a.columns / 2);
    Matrix B21(a.columns / 2, a.columns / 2);
    Matrix B22(a.columns / 2, a.columns / 2);

    GetQuarterOf(A11, a, 1, 1);
    GetQuarterOf(A12, a, 1, 2);
    GetQuarterOf(A21, a, 2, 1);
    GetQuarterOf(A22, a, 2, 2);

    GetQuarterOf(B11, b, 1, 1);
    GetQuarterOf(B12, b, 1, 2);
    GetQuarterOf(B21, b, 2, 1);
    GetQuarterOf(B22, b, 2, 2);

    Matrix P1 = getMatrixMulStrassen(A11 + A22, B11 + B22);
    Matrix P2 = getMatrixMulStrassen(A21 + A22, B11);
    Matrix P3 = getMatrixMulStrassen(A11, B12 - B22);
    Matrix P4 = getMatrixMulStrassen(A22, B21 - B11);
    Matrix P5 = getMatrixMulStrassen(A11 + A12, B22);
    Matrix P6 = getMatrixMulStrassen(A21 - A11, B11 + B12);
    Matrix P7 = getMatrixMulStrassen(A12 - A22, B21 + B22);

    Matrix C11 = P1 + P4 - P5 + P7;
    Matrix C12 = P3 + P5;
    Matrix C21 = P2 + P4;
    Matrix C22 = P1 - P2 + P3 + P6;

    return assembleMatrix(C11, C12, C21, C22);
}

Matrix getParallelMatrixMul(const Matrix & a, const Matrix & b)
{
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // calculating delta
    int delta = 7 / size;
    int tail = 7 % size;
    // num of mults
    int count;
    if (rank < tail)  count = delta + 1;
    else count = delta;

    // Work division
    if (rank == 0) {
        Matrix A11(a.columns / 2, a.columns / 2);
        Matrix A12(a.columns / 2, a.columns / 2);
        Matrix A21(a.columns / 2, a.columns / 2);
        Matrix A22(a.columns / 2, a.columns / 2);

        Matrix B11(a.columns / 2, a.columns / 2);
        Matrix B12(a.columns / 2, a.columns / 2);
        Matrix B21(a.columns / 2, a.columns / 2);
        Matrix B22(a.columns / 2, a.columns / 2);

        GetQuarterOf(A11, a, 1, 1);
        GetQuarterOf(A12, a, 1, 2);
        GetQuarterOf(A21, a, 2, 1);
        GetQuarterOf(A22, a, 2, 2);

        GetQuarterOf(B11, b, 1, 1);
        GetQuarterOf(B12, b, 1, 2);
        GetQuarterOf(B21, b, 2, 1);
        GetQuarterOf(B22, b, 2, 2);

        Matrix matrices[7][2];

        // each process will solve ~7^k/rank mults
        matrices[0][0] = A11 + A22; matrices[0][1] = B11 + B22;
        matrices[1][0] = A21 + A22; matrices[1][1] = B11;
        matrices[2][0] = A11;       matrices[2][1] = B12 - B22;
        matrices[3][0] = A22;       matrices[3][1] = B21 - B11;
        matrices[4][0] = A11 + A12; matrices[4][1] = B22;
        matrices[5][0] = A21 - A11; matrices[5][1] = B11 + B12;
        matrices[6][0] = A12 - A22; matrices[6][1] = B21 + B22;

        int m_size = A11.columns;
        MPI_Bcast(&m_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        int proc;
        for (proc = 1; proc < tail; proc++) {
            for (int i = 0; i < delta + 1; i++) {
                MPI_Send(matrices[proc*(delta + 1) + i][0].buf, m_size * m_size, MPI_DOUBLE, proc, 1, MPI_COMM_WORLD);
                MPI_Send(matrices[proc*(delta + 1) + i][1].buf, m_size * m_size, MPI_DOUBLE, proc, 2, MPI_COMM_WORLD);
            }
        }
        for (; proc < size; proc++) {
            for (int i = 0; i < delta; i++) {
                MPI_Send(matrices[tail*(delta + 1) + (proc - tail)*delta + i][0].buf,
                    m_size * m_size, MPI_DOUBLE, proc, 1, MPI_COMM_WORLD);
                MPI_Send(matrices[tail*(delta + 1) + (proc - tail)*delta + i][1].buf,
                    m_size * m_size, MPI_DOUBLE, proc, 2, MPI_COMM_WORLD);
            }
        }

        // gathering results mults
        cout << "gathering results mults" << endl;
        Matrix P[7];
        // local 0th rank computations
        for (int i = 0; i < count; i++) {
            P[i] = getMatrixMulStrassen(matrices[i][0], matrices[i][1]);
        }
        MPI_Status status;

        for (proc = 1; proc < tail; proc++) {
            for (int i = 0; i < delta + 1; i++) {
                P[proc*(delta + 1) + i] = Matrix(m_size, m_size);
                MPI_Recv(P[proc*(delta + 1) + i].buf, m_size * m_size, MPI_DOUBLE, proc, 3, MPI_COMM_WORLD, &status);
            }
        }
        for (; proc < size; proc++) {
            for (int i = 0; i < delta; i++) {
                P[tail*(delta + 1) + (proc - tail)*delta + i] = Matrix(m_size, m_size);
                MPI_Recv(P[tail*(delta + 1) + (proc - tail)*delta + i].buf, m_size * m_size,
                    MPI_DOUBLE, proc, 3, MPI_COMM_WORLD, &status);
            }
        }

        // calculating final result
        cout << "calc final result" << endl;
        Matrix C11 = P[0] + P[3] - P[4] + P[6];
        Matrix C12 = P[2] + P[4];
        Matrix C21 = P[1] + P[3];
        Matrix C22 = P[0] - P[1] + P[2] + P[5];

        return assembleMatrix(C11, C12, C21, C22);

    } else { // All process with rank != 0 just count received mults
        int m_size;
        MPI_Bcast(&m_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        // local sub matrices
        Matrix * l_a = new Matrix[count];
        Matrix * l_b = new Matrix[count];
        MPI_Status status;
        for (int i = 0; i < count; i++) {
            l_a[i] = Matrix(m_size, m_size);
            MPI_Recv(l_a[i].buf, m_size * m_size, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
            l_b[i] = Matrix(m_size, m_size);
            MPI_Recv(l_b[i].buf, m_size * m_size, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);
        }
        
        Matrix * mults = new Matrix[count];
        for (int i = 0; i < count; i++) {
            mults[i] = getMatrixMulStrassen(l_a[i], l_b[i]);
            MPI_Send(mults[i].buf, m_size * m_size, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
        }

        return Matrix();
    }
}

void GetQuarterOf(Matrix & recv, const Matrix & src, int y, int x) {
    x--; y--;
    int size = src.columns / 2;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            recv.buf[i * size + j] = src.buf[size*size*2*y + x*size + i * size * 2 + j];
        }
}

void SetQuarterTo(Matrix & recv, const Matrix & src, int y, int x) {
    x--; y--;
    int size = src.columns;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            recv.buf[size*size * 2 * y + x * size + i * size * 2 + j] = src.buf[i * size + j];
        }
}

Matrix assembleMatrix(const Matrix & c11, const Matrix & c12, const Matrix & c21, const Matrix & c22) {
    Matrix res(c11.rows * 2, c11.columns * 2);
    SetQuarterTo(res, c11, 1, 1);
    SetQuarterTo(res, c12, 1, 2);
    SetQuarterTo(res, c21, 2, 1);
    SetQuarterTo(res, c22, 2, 2);
    return res;
}
