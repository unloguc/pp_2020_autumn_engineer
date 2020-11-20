// Copyright 2020 Khismatulina Karina
#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <climits>
#include <cassert>
#include "../../../modules/task_2/khismatulina_k_contrast_raise/contrast_raise.h"

std::vector<int> getRandomImage(int size) {
    assert(size > 0);
    if (size <= 0) {
        throw "fucking error";
    }
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::uniform_int_distribution<> dist(0, 255);
    std::vector<int> imageData(256);
    for (int i = 0; i < size; ++i) {
        imageData[i] = dist(gen) % 256;
    }
    return imageData;
}

std::vector<int> contrastRaiseSeq(std::vector<int> imageData, int size, int contrast) {
    std::vector<int> buf(256);
    int midBright = 0;
    for (int i = 0; i < size; ++i) {
        midBright += imageData[i];
    }
    midBright /= size;
    float corr = 1.0 + static_cast<float>(contrast) / 100;
    for (int i = 0; i < 256; ++i) {
        int a = ((i - midBright) * corr) + midBright;
        if (a < 0) buf[i] = 0;
        else if (a > 255) buf[i] = 255;
        else buf[i] = a;
    }
    for (int i = 0; i < size; ++i) {
        imageData[i] = buf[imageData[i]];
    }
    imageData.resize(size);
    return imageData;
}

std::vector<int> contrastRaiseParallel(std::vector<int> imageData, int imageSize, int contrast) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int delta = imageSize / size;
    int rem = imageSize - delta * size;

    if (rank == 0) {
        if (delta > 0) {
            for (int proc = 1; proc < size; ++proc) {
                MPI_Send(&imageData[proc * delta + rem], delta, MPI_INT, proc, 0, MPI_COMM_WORLD);
            }
        }
    }

    int tmp = 0;
    std::vector<int> buf_1(imageSize, 0);
    if (rank == 0) {
        for (int i = 0; i < rem + delta; ++i) {
            tmp += imageData[i];
        }
    } else {
        if (delta > 0) {
            buf_1.resize(delta);
            MPI_Status status;
            MPI_Recv(&buf_1[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            for (int i = 0; i < delta; ++i) {
                tmp += buf_1[i];
            }
        }
    }

    int pixelsSum = 0;
    std::vector<int> buf_2(256, 0);
    MPI_Reduce(&tmp, &pixelsSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double cont = 1.0 + contrast / 100.0;
        int midBright = pixelsSum / imageSize;
        for (int i = 0; i < 256; ++i) {
            buf_2[i] = static_cast<int>(midBright + cont * (static_cast<int>(i) - midBright));

            if (buf_2[i] <= 0)
                buf_2[i] = 0;

            if (buf_2[i] >= 255)
                buf_2[i] = 255;
        }
    }

    MPI_Bcast(&buf_2[0], 256, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> localRes(delta, 0);
    std::vector<int> res(imageSize, 0);
    if (rank == 0) {
        for (int i = 0; i < delta + rem; ++i) {
            res[i] = buf_2[imageData[i]];
        }
    } else {
        for (int i = 0; i < delta; ++i) {
            localRes[i] = buf_2[buf_1[i]];
        }
    }

    if (rank != 0) {
        MPI_Send(&localRes[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    std::vector<int> tmp2(delta, 0);

    if (rank == 0) {
        for (int proc = 1; proc < size; ++proc) {
            MPI_Status status;
            MPI_Recv(&tmp2[0], delta, MPI_INT, proc, 0, MPI_COMM_WORLD, &status);
            for (int i = 0; i < delta; ++i) {
                res[proc * delta + i + rem] = tmp2[i];
            }
        }
    }
    return res;
}
