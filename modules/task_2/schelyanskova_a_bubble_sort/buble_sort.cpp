// Copyright 2020 Schelyanskova Anastasiya
#include "../../../modules/task_2/schelyanskova_a_bubble_sort/buble_sort.h"
#include <algorithm>
#include <utility>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>


std::vector<int> getRandomVector(const int& size) {
    if (size <= 0)
        throw "error";
    std::vector<int> randvec(size);
    std::default_random_engine random;
    random.seed(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < size; i++)
        i = random() % 1000;
    return randvec;
}

std::vector<int> sequentialBubbleSort(const std::vector<int>& vec) {
    if (static_cast<int>(vec.size()) == 0) {
        throw "error";
    }
    std::vector<int> sorted(vec);
    for (int i = 0; i < static_cast<int>(vec.size() - 1); ++i) {
        for (int j = 0; j < static_cast<int>(vec.size() - 1); ++j) {
            if (sorted[j] > sorted[j + 1]) {
                std::swap(sorted.at(j), sorted.at(j + 1));
            }
        }
    }
    return sorted;
}

std::vector<int> parallelOddEvenSort(std::vector<int> vec, const int& size) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int codeErr = 0;
    if (ProcRank == 0) {
        if (static_cast<int>(vec.size()) == 0 || static_cast<int>(vec.size()) != size)
            codeErr = -1;
        for (int prcs = 1; prcs < ProcNum; prcs++) {
            MPI_Send(&codeErr, 1, MPI_INT, prcs, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Status status;
        MPI_Recv(&codeErr, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    if (codeErr == -1)
        throw - 1;
    int elem_for_proc = size / ProcNum;
    int rem = size % ProcNum;
    std::vector<int> local_vec(elem_for_proc);
    std::vector<int> temp(elem_for_proc);
    vec.resize(size);
    if (elem_for_proc != 0)
        MPI_Scatter(&vec[0], elem_for_proc, MPI_INT, &local_vec[0], elem_for_proc, MPI_INT, 0, MPI_COMM_WORLD);
    if (ProcRank == 0 && rem != 0) {
        local_vec.insert(local_vec.end(), vec.begin() + (size - rem), vec.end());
    }
    std::vector<int> res;
    local_vec = sequentialBubbleSort(local_vec);
    for (int i = 0; i < ProcNum; ++i) {
        int partner = computePartner(i, ProcRank);
        if (partner < 0 || partner >= ProcNum) {
            continue;
    }
        MPI_Status stats;
        if (ProcRank == 0)
            MPI_Sendrecv(&local_vec[0] + rem, elem_for_proc, MPI_INT, partner, 1, &temp[0],
                elem_for_proc, MPI_INT, partner, 1, MPI_COMM_WORLD, &stats);
        else
            MPI_Sendrecv(&local_vec[0], elem_for_proc, MPI_INT, partner, 1, &temp[0],
                elem_for_proc, MPI_INT, partner, 1, MPI_COMM_WORLD, &stats);
        res.resize(local_vec.size() + temp.size());
        std::merge(local_vec.begin(), local_vec.end(), temp.begin(), temp.end(), res.begin(),
            [](const int& a, const int& b) {return a < b; });
        if (ProcRank < partner) {
            int iter;
            if (ProcRank == 0)
                iter = rem + elem_for_proc;
            else
                iter = elem_for_proc;
            local_vec = std::vector<int>(res.begin(), res.begin() + iter);
        } else {
            local_vec = std::vector<int>(res.end() - elem_for_proc, res.end());
        }
    }
    if (ProcRank == 0) {
        MPI_Status stat;
        res = local_vec;
        for (int i = 1; i < ProcNum; ++i) {
            MPI_Recv(&temp[0], elem_for_proc, MPI_INT, i, 2, MPI_COMM_WORLD, &stat);
            res.insert(res.end(), temp.begin(), temp.end());
        }
    } else {
        MPI_Send(&local_vec[0], elem_for_proc, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
    return res;
}

int computePartner(const int& iter, const int& rank) {
    int res;
    if (iter % 2 == 0) {
        if (rank % 2 == 0) {
         res = rank + 1;
        } else {
         res = rank - 1;
        }
    } else {
        if (rank % 2 == 0) {
         res = rank - 1;
        } else {
         res = rank + 1;
        }
    }
    return res;
}
