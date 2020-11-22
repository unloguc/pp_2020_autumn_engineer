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
        rowOrder[i] = i * size;
    }

    // First step - Convert to upper triangular matrix
    int coef = 0;
    for (int equat = 0; equat < size; ++equat) {
        // Find pivot row
        int pivRow = FindPivotRows(coefs, rowOrder, size, equat);
        if (rowOrder[equat] != pivRow - equat)
            std::swap(rowOrder[equat], rowOrder[pivRow / size]);

        // Calculate coefs
        double multi = coefs[rowOrder[equat] + equat];
        for (int curCoefs = equat; curCoefs < size; ++curCoefs) {
            coefs[rowOrder[equat] + curCoefs] /= multi;
        }
        rightPart[rowOrder[equat] / size] /= multi;

        for (int eq = equat + 1; eq < size; ++eq) {
            multi = coefs[rowOrder[eq] + equat];
            for (int curCoefs = equat; curCoefs < size; ++curCoefs) {
                coefs[rowOrder[eq] + curCoefs] -= coefs[rowOrder[equat] + curCoefs] * multi;
            }
            rightPart[rowOrder[eq] / size] -= rightPart[rowOrder[equat] / size] * multi;
        }
    }
    ++coef;
    // Second step - revert
    for (int curEquat = size - 1; curEquat > -1; --curEquat) {
        int index = rowOrder[curEquat] / size;
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
    double MaxValue;
    int PivotPos = 0;
    pairMP globInd;
    std::vector<double> pivotRow(size + 1);

    std::vector<double> resVec(size);
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

    // The iterations of the Gaussian elimination stage
    for (int i = 0; i < size; ++i) {
        // Calculating the local pivot row
       MaxValue = 0;
       for (int j = 0; j < countRow; ++j) {
           if ((locOrder[j] == -1) && (MaxValue < fabs(recCoefsVec[j * size + i]))) {
               MaxValue = fabs(recCoefsVec[j * size + i]);
               PivotPos = j;
           }
       }
       locInd.proc = ProcRank;
       locInd.Max = MaxValue;
        MPI_Allreduce(&locInd, &globInd, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
        if (ProcRank == globInd.proc) {
            locOrder[PivotPos] = i;
            rowOrder[i] = sendRightPOffset[ProcRank] + PivotPos;
        }
        MPI_Bcast(&rowOrder[i], 1, MPI_DOUBLE, globInd.proc, MPI_COMM_WORLD);

        if (ProcRank == globInd.proc) {
            double multi = recCoefsVec[PivotPos * size + i];
            for (int coef = i; coef < size; ++coef)
                recCoefsVec[PivotPos * size + coef] /= multi;
            recRightPVec[PivotPos] /= multi;
            auto it = recCoefsVec.begin();
            pivotRow.assign(it + PivotPos * size, it + PivotPos * size + size);
             pivotRow.push_back(recRightPVec[PivotPos]);
        }
        MPI_Bcast(&pivotRow[0], size + 1, MPI_DOUBLE, globInd.proc, MPI_COMM_WORLD);
        double multi;
        for (size_t eq = 0; eq < locOrder.size(); ++eq) {
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
    int curIndMax = rowOrder[curEquat] + curEquat;
    for (int j = curEquat + 1; j < size; ++j) {
        if (fabs(coefs[rowOrder[j] + curEquat]) > fabs(coefs[curIndMax])) {
            curIndMax = rowOrder[j] + curEquat;
        }
    }
    return curIndMax;
}
