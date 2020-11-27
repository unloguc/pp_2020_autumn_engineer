  // Copyright 2020 Maksimov Andrey

#include <mpi.h>
#include <time.h>
#include <random>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include "../../modules/task_3/maksimov_a_mark_components/mark_components.h"

std::vector<uint32_t> getRandomBinaryImage(int height, int width) {
    if (height < 1 || width < 1) {
        throw "height or width < 1 error";
    }

    std::mt19937 gen;
    gen.seed(time(0));

    std::vector<uint32_t> image(height * width);
    for (int i = 0; i < height * width; ++i)
        image[i] = gen() % 2;

    return image;
}

inline std::vector<uint32_t> markComponentsBlock(
    std::vector<uint32_t> image, int height, int width, uint32_t curr = 1) {
    int elemBColumn = 0;
    int elemCRow = 0;
    uint32_t A, B, C;

    for (int row = 0; row < height; ++row) {
        for (int column = 0; column < width; ++column) {
            elemCRow = column - 1;
            if (elemCRow < 0) {
                elemCRow = 0;
                B = 0;
            } else {
                B = image[row * width + elemCRow];
            }
            elemBColumn = row - 1;
            if (elemBColumn < 0) {
                elemBColumn = 0;
                C = 0;
            } else {
                C = image[elemBColumn * width + column];
            }
            A = image[row * width + column];
            if (A == 0) {
            } else if (B == 0 && C == 0) {
                ++curr;
                image[row * width + column] = curr;
            } else if (B != 0 && C == 0) {
                image[row * width + column] = B;
            } else if (B == 0 && C != 0) {
                image[row * width + column] = C;
            } else if (B != 0 && C != 0) {
                if (B == C) {
                    image[row * width + column] = B;
                } else {
                    image[row * width + column] = B;
                    for (int i = 0; i < height; ++i) {
                        for (int j = 0; j < width; ++j) {
                            if (image[i * width + j] == C) {
                                image[i * width + j] = B;
                            }
                        }
                    }
                }
            }
        }
    }
    return image;
}

inline std::vector<uint32_t> renumberComponents(std::vector<uint32_t> image, int height, int width) {
    std::vector<uint32_t> source = std::vector<uint32_t>();
    std::vector<uint32_t> result = std::vector<uint32_t>();

    int count = 0;
    int size = height * width;

    for (int i = 0; i < size; ++i) {
        if (image[i] != 0
            && std::find(source.begin(), source.end(), image[i]) == source.end()) {
            source.push_back(image[i]);
            ++count;
            result.push_back(count);
        }
    }
    for (int i = 0; i < size; ++i) {
        if (image[i] != 0) {
            image[i] = result[std::distance(source.begin(), std::find(source.begin(), source.end(), image[i]))];
        }
    }
    return image;
}

inline std::vector<uint32_t> rotateImage(std::vector<uint32_t> image, int height, int width, bool clockwise) {
    std::vector<uint32_t> rotatedImage(height * width);
    if (clockwise) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                rotatedImage[j * height + height - 1 - i] = image[i * width + j];
            }
        }
    } else {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                rotatedImage[(width - j - 1) * height + i] = image[i * width + j];
            }
        }
    }
    return rotatedImage;
}

std::vector<uint32_t> markComponentsNotParall(std::vector<uint32_t> image, int height, int width) {
    image = markComponentsBlock(image, height, width);
    image = renumberComponents(image, height, width);
    return image;
}

std::vector<uint32_t> markComponents(std::vector<uint32_t> image, int height, int width) {
    MPI_Status mpiStatus;
    int procNum, procRank, heightPart, heightPartForOthers, size, sizeForOthers;
    bool isRotated = false;

    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    if (procNum != 1) {
        const double ratio = 0.6;
        if (width > 500 && static_cast<double>(height) / width < ratio) {
            if (procRank == 0) {
                image = rotateImage(image, height, width, true);
            }
            int temp = height;
            height = width;
            width = temp;
            isRotated = true;
        }
        if (procRank == 0) {
            heightPart = height / procNum + height % procNum;
        } else {
            heightPart = height / procNum + 1;
        }
    } else {
        heightPart = height;
    }
    size = heightPart * width;

    if (procRank == 0) {
        heightPartForOthers = height / procNum + 1;
        sizeForOthers = heightPartForOthers * width;
        for (int currProc = 1; currProc < procNum; ++currProc) {
            MPI_Send(image.data() + size - width + (currProc - 1) * (sizeForOthers - width), sizeForOthers,
                MPI_INT, currProc, 0, MPI_COMM_WORLD);
        }
    } else {
        image = std::vector<uint32_t>(size);
        MPI_Recv(image.data(), size, MPI_INT, 0, 0, MPI_COMM_WORLD, &mpiStatus);
    }

    if (procRank == 0) {
        image = markComponentsBlock(image, heightPart, width);
    } else {
        const uint32_t indent = 4000000;
        image = markComponentsBlock(image, heightPart, width, indent * procRank + 1);
    }
    if (procRank != 0) {
        MPI_Send(image.data(), size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        int indexOfLastHeightRow = heightPart - 1;

        for (int currProc = 1; currProc < procNum; ++currProc) {
            // Копируем общую строчку
            std::vector<uint32_t> commonRow = std::vector<uint32_t>(
                image.data() + indexOfLastHeightRow * width,
                image.data() + indexOfLastHeightRow * width + width);

            MPI_Recv(image.data() + indexOfLastHeightRow * width, sizeForOthers,
                MPI_INT, currProc, 0, MPI_COMM_WORLD, &mpiStatus);

            // Тут одинаковые значения из скопированной строки (по информации полученной строки)
            std::unordered_set<uint32_t> commonRowValues = std::unordered_set<uint32_t>();
            // Тут одинаковые значения из полученной строки (по информации скопированной строки)
            std::unordered_set<uint32_t> newRowValues = std::unordered_set<uint32_t>();

            // Проходим по общей строчке
            for (int c1 = 0; c1 < width; ++c1) {
                if (commonRow[c1] != 0 && commonRow[c1] != image[indexOfLastHeightRow * width + c1]) {
                    commonRowValues.insert(commonRow[c1]);
                    newRowValues.insert(image[indexOfLastHeightRow * width + c1]);
                    for (int c2 = 0; c2 < width; ++c2) {
                        uint32_t commonRowValuesSize = commonRowValues.size();
                        uint32_t newRowValuesSize = newRowValues.size();
                        for (int c3 = 0; c3 < width; ++c3) {
                            if (std::find(commonRowValues.begin(), commonRowValues.end(), commonRow[c3])
                                != commonRowValues.end()) {
                                newRowValues.insert(image[indexOfLastHeightRow * width + c3]);
                            }
                            if (std::find(newRowValues.begin(), newRowValues.end(),
                                image[indexOfLastHeightRow * width + c3]) != newRowValues.end()) {
                                commonRowValues.insert(commonRow[c3]);
                            }
                        }
                        if (commonRowValuesSize == commonRowValues.size() && newRowValuesSize == newRowValues.size()) {
                            break;
                        }
                    }
                    int commonRowMainValue = commonRow[c1];
                    if (commonRowValues.size() > 1) {
                        commonRowValues.erase(commonRowMainValue);
                        // Цикл до общей строки
                        for (int index = 0; index < indexOfLastHeightRow * width; ++index) {
                            if (std::find(commonRowValues.begin(), commonRowValues.end(), image[index])
                                != commonRowValues.end()) {
                                image[index] = commonRowMainValue;
                            }
                        }
                        // Цикл по скопированной строчке
                        for (int index = 0; index < width; ++index) {
                            if (std::find(commonRowValues.begin(), commonRowValues.end(), commonRow[index])
                                != commonRowValues.end()) {
                                commonRow[index] = commonRowMainValue;
                            }
                        }
                    }
                    for (int index = indexOfLastHeightRow * width;
                        index < (indexOfLastHeightRow + heightPartForOthers) * width; ++index) {
                        if (std::find(newRowValues.begin(), newRowValues.end(), image[index])
                            != newRowValues.end()) {
                            image[index] = commonRowMainValue;
                        }
                    }
                    commonRowValues.clear();
                    newRowValues.clear();
                }
            }
            indexOfLastHeightRow += heightPartForOthers - 1;
        }
        if (isRotated) {
            image = rotateImage(image, height, width, false);
            int temp = height;
            height = width;
            width = temp;
        }
        image = renumberComponents(image, height, width);
    }
    return image;
}
