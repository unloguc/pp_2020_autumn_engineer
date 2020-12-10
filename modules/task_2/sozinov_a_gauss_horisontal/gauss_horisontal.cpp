// Copyright 2020 Sozinov Alex

#include <mpi.h>
#include <algorithm>
#include <utility>
#include <cmath>
#include <vector>

#include "../../modules/task_2/sozinov_a_gauss_horisontal/gauss_horisontal.h"

struct pairMP {
    double Max;
    int proc;
};

std::vector<double> GetSequentialSolution(const std::vector<double>& _coefs,
    const std::vector<double>& _rightPart, int size) {
    std::vector<double> coefs(_coefs);
    std::vector<double> rightPart(_rightPart);
    // result vector
    std::vector<double> resVec(size);
    // index - number iteration, value - number first coef in equation
    std::vector<int> rowOrder(size);
    for (int i = 0; i < size; ++i) {
        rowOrder[i] = i;
    }
    // First step - Convert to upper triangular matrix
    int coef = 0;
    for (int equat = 0; equat < size; ++equat) {
        // Find pivot row
        int pivRow = FindPivotRows(coefs, rowOrder, size, equat);
        std::vector<int>::iterator it = rowOrder.begin();
        int index = std::find(it + equat, it + size, pivRow) - it;
        std::swap(rowOrder[equat], rowOrder[index]);
        // Calculate coefs
        double multi = coefs[rowOrder[equat] * size + equat];
        for (int curCoefs = equat; curCoefs < size; ++curCoefs) {
            coefs[rowOrder[equat] * size + curCoefs] /= multi;
        }
        rightPart[rowOrder[equat]] /= multi;

        for (int eq = equat + 1; eq < size; ++eq) {
            multi = coefs[rowOrder[eq] * size + equat];
            for (int curCoefs = equat; curCoefs < size; ++curCoefs) {
                coefs[rowOrder[eq] * size + curCoefs] -= coefs[rowOrder[equat] * size + curCoefs] * multi;
            }
            rightPart[rowOrder[eq]] -= rightPart[rowOrder[equat]] * multi;
        }
    }
    ++coef;
    // Second step - revert
    for (int curEquat = size - 1; curEquat > -1; --curEquat) {
        int index = rowOrder[curEquat];
        resVec[curEquat] = rightPart[index];
        for (int curCoefs = curEquat + 1; curCoefs < size; ++curCoefs) {
            resVec[curEquat] -= coefs[index * size + curCoefs] * resVec[curCoefs];
        }
    }
    return resVec;
}

std::vector<double> GetParalSolution(const std::vector<double>& _coefs,
    const std::vector<double>& _rightPart, int size) {
    std::vector<double> coefs(_coefs);
    std::vector<double> rightPart(_rightPart);
    int ProcRank;
    int ProcCount;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcCount);
    double maxCoef = 0;
    int pivotPos = 0;
    pairMP globInd = { 0.0, 0 };
    std::vector<double> pivotRow(size + 1);

    std::vector<double> resVec(size, 0);
    std::vector<int> rowOrder(size, -1);
    int offset = size / ProcCount;
    std::vector<int> sendCoefSize(ProcCount, offset * size);
    std::vector<int> sendRightPSize(ProcCount, offset);
    for (int i = 0; i < size % ProcCount; ++i) {
        sendCoefSize[i] += size;
        ++sendRightPSize[i];
    }
    if (size % ProcCount)
        ++offset;
    std::vector<double> recCoefsVec(offset * size);
    std::vector<double> recRightPVec(offset);
    std::vector<int> sendCoefOffset(ProcCount);
    std::vector<int> sendRightPOffset(ProcCount);
    for (int posC = 0, posR = 0, i = 0; i < ProcCount; ++i) {
        sendCoefOffset[i] = posC;
        sendRightPOffset[i] = posR;
        posC += sendCoefSize[i];
        posR += sendRightPSize[i];
    }

    MPI_Scatterv(&coefs[0], &sendCoefSize[0], &sendCoefOffset[0], MPI_DOUBLE,
        &recCoefsVec[0], sendCoefSize[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(&rightPart[0], &sendRightPSize[0], &sendRightPOffset[0], MPI_DOUBLE,
        &recRightPVec[0], sendRightPSize[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);


    pairMP locInd;
    int countRow = sendRightPSize[ProcRank];
    std::vector<int> locOrder(countRow, -1);

    // First step - convert to upper triangular matrix
    for (int i = 0; i < size; ++i) {
        // Find local pivot row
        maxCoef = 0;
        for (int j = 0; j < countRow; ++j) {
            if ((locOrder[j] == -1) && (maxCoef < fabs(recCoefsVec[j * size + i]))) {
                maxCoef = fabs(recCoefsVec[j * size + i]);
                pivotPos = j;
            }
        }
        locInd.proc = ProcRank;
        locInd.Max = maxCoef;
        MPI_Allreduce(&locInd, &globInd, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
        if (ProcRank == globInd.proc) {
            locOrder[pivotPos] = i;
            rowOrder[i] = sendRightPOffset[ProcRank] + pivotPos;
            double multi = recCoefsVec[pivotPos * size + i];
            for (int coef = i; coef < size; ++coef)
                recCoefsVec[pivotPos * size + coef] /= multi;
            recRightPVec[pivotPos] /= multi;
            auto it = recCoefsVec.begin();
            pivotRow.assign(it + pivotPos * size, it + pivotPos * size + size);
            pivotRow.push_back(recRightPVec[pivotPos]);
        }
        MPI_Bcast(&rowOrder[i], 1, MPI_DOUBLE, globInd.proc, MPI_COMM_WORLD);
        MPI_Bcast(&pivotRow[0], size + 1, MPI_DOUBLE, globInd.proc, MPI_COMM_WORLD);
        double multi;
        for (int eq = 0; eq < countRow; ++eq) {
            if (locOrder[eq] == -1) {
                multi = recCoefsVec[eq * size + i] / pivotRow[i];
                for (int cf = i; cf < size; ++cf) {
                    recCoefsVec[eq * size + cf] -= pivotRow[cf] * multi;
                }
                recRightPVec[eq] -= pivotRow[size] * multi;
            }
        }
    }
    MPI_Gatherv(&recCoefsVec[0], sendCoefSize[ProcRank], MPI_DOUBLE,
        &coefs[0], &sendCoefSize[0], &sendCoefOffset[0], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(&recRightPVec[0], sendRightPSize[ProcRank], MPI_DOUBLE,
        &rightPart[0], &sendRightPSize[0], &sendRightPOffset[0], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Second step - revert
    if (ProcRank == 0) {
        for (int curEquat = size - 1; curEquat > -1; --curEquat) {
            int index = rowOrder[curEquat];
            resVec[curEquat] = rightPart[index];
            for (int curCoefs = curEquat + 1; curCoefs < size; ++curCoefs) {
                resVec[curEquat] -= coefs[index * size + curCoefs] * resVec[curCoefs];
            }
        }
    }
    return resVec;
}

int FindPivotRows(const std::vector<double>& coefs, const std::vector<int>& rowOrder, int size, int curEquat) {
    int curIndMax = rowOrder[curEquat] * size + curEquat;
    for (int j = curEquat + 1; j < size; ++j) {
        if (fabs(coefs[rowOrder[j] * size + curEquat]) > fabs(coefs[curIndMax])) {
            curIndMax = rowOrder[j] * size + curEquat;
        }
    }
    return curIndMax / size;
}
