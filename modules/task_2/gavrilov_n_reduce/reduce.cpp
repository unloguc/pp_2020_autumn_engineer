// Copyright 2020 Gavrilov Nikita
#include <mpi.h>
#include <algorithm>
#include <cstring>
#include "../../modules/task_2/gavrilov_n_reduce/reduce.h"

void operation(MPI_Datatype datatype, void* arr1, void* arr2, int count, MPI_Op op) {
    if (datatype == MPI_INT)
        operation(static_cast<int*>(arr1), static_cast<int*>(arr2), count, op);
    else if (datatype == MPI_FLOAT)
        operation(static_cast<float*>(arr1), static_cast<float*>(arr2), count, op);
    else if (datatype == MPI_DOUBLE)
        operation(static_cast<double*>(arr1), static_cast<double*>(arr2), count, op);
    else
        throw "Type not supported.";
}

void* createBuf(MPI_Datatype datatype, int count) {
    if (datatype == MPI_INT)
        return new int[count];
    else if (datatype == MPI_FLOAT)
        return new float[count];
    else if (datatype == MPI_DOUBLE)
        return new double[count];
    else
        throw "Type not supported.";
}

void copy(MPI_Datatype datatype, int count, void* dest, void* src) {
    int size;
    if (datatype == MPI_INT)
        size = sizeof(int);
    else if (datatype == MPI_FLOAT)
        size = sizeof(float);
    else if (datatype == MPI_DOUBLE)
        size = sizeof(double);
    else
        throw "Type not supported.";
    std::memcpy(dest, src, count * size);
}

void deleteBuf(MPI_Datatype datatype, void* buf) {
    if (buf == nullptr)
        return;

    if (datatype == MPI_INT)
        delete[] static_cast<int*>(buf);
    else if (datatype == MPI_FLOAT)
        delete[] static_cast<float*>(buf);
    else if (datatype == MPI_DOUBLE)
        delete[] static_cast<double*>(buf);
    else
        throw "Type not supported.";
}

void reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (root < 0 || root >= size)
        throw "wrong root";

    int index = (rank - root + size) % size;

    int magicNumber = 2;

    void* rbuf = createBuf(datatype, count);
    void* sbuf = createBuf(datatype, count);
    copy(datatype, count, sbuf, sendbuf);

    while (true) {
        if (index % magicNumber == 0) {
            MPI_Status status;

            int recvIdRooted = index + magicNumber / 2;
            int recvId = (recvIdRooted + root) % size;

            if (recvIdRooted < size) {
                MPI_Recv(rbuf, count, datatype, recvId, 0, comm, &status);
                operation(datatype, sbuf, rbuf, count, op);
            }

            if (magicNumber >= size) {
                copy(datatype, count, recvbuf, sbuf);
                break;
            }

            magicNumber *= 2;
        } else {
            int sendIdRooted = index - magicNumber / 2;
            int sendId = (sendIdRooted + root) % size;

            MPI_Send(sbuf, count, datatype, sendId, 0, comm);
            break;
        }
    }

    deleteBuf(datatype, rbuf);
    deleteBuf(datatype, sbuf);
}
