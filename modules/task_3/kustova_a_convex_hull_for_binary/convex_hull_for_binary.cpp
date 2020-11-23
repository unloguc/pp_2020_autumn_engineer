// Copyright 2020 Kustova Anastasiya
#include "../../../modules/task_3/kustova_a_convex_hull_for_binary/convex_hull_for_binary.h"
#include <mpi.h>
#include <vector>
#include <random>
#include <iostream>
#include <ctime>
void printImage(int ** image, int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << image[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int giveNumbersToComponents(int ** image, int height, int width) {
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int componentsNumber = 0;
    const int delta_height = height / size;
    const int rem_height = height % size;
    int start_height;
    if (rank < rem_height) {
        start_height = rank * (delta_height + 1);
    } else {
        start_height = delta_height * rank + rem_height;
    }
    if (image[start_height][0] != 0) {
        componentsNumber++;
        image[start_height][0] = componentsNumber;
    }
    for (int j = 1; j < width; j++) {
        if (image[start_height][j] != 0) {
            if (image[start_height][j - 1] != 0) {
                image[start_height][j] = image[start_height][j - 1];
            } else {
                componentsNumber++;
                image[start_height][j] = componentsNumber;
            }
        }
    }
    int rem_count;
    if (rank < rem_height) {
        rem_count = delta_height + 1;
    } else {
        rem_count = delta_height;
    }
    rem_count += start_height;
    for (int i = start_height + 1; i < rem_count; i++) {
        for (int j = 0; j < width; j++) {
            if (image[i][j] != 0) {
                if ((j > 0) && (image[i - 1][j - 1] != 0)) {
                    image[i][j] = image[i - 1][j - 1];
                } else if (image[i - 1][j] != 0) {
                    image[i][j] = image[i - 1][j];
                } else if ((j < width - 1) && (image[i - 1][j + 1] != 0)) {
                    image[i][j] = image[i - 1][j + 1];
                } else if ((j > 0) && (image[i][j - 1] != 0)) {
                    image[i][j] = image[i][j - 1];
                } else {
                    componentsNumber++;
                    image[i][j] = componentsNumber;
                }
            }
        }
    }

    if (size > 1) {
        if (rank == 0) {
            std::vector<int> send(1);
            send[0] = componentsNumber;
            MPI_Send(&send[0], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Status status;
            MPI_Recv(&send[0], 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &status);
            componentsNumber = send[0];
        } else {
            MPI_Status status;
            int cNumber;
            std::vector<int> send(1);
            MPI_Recv(&send[0], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
            cNumber = send[0];
            componentsNumber += cNumber;
            send[0] = componentsNumber;
            if (rank < size - 1) {
                MPI_Send(&send[0], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            } else {
                MPI_Send(&send[0], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
            for (int i = start_height; i < rem_count; i++) {
                for (int j = 0; j < width; j++) {
                    if (image[i][j] != 0) {
                        image[i][j] += cNumber;
                    }
                }
            }
        }
        std::vector<int> send(height*width);
        int k = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                send[k] = image[i][j];
                k++;
            }
        }

        std::vector<int> recv(height*width);
        MPI_Allreduce(&send[0], &recv[0], height*width, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        k = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j] = recv[k];
                k++;
            }
        }
        std::vector<int> number(1);
        if (rank == 0) {
            number[0] = componentsNumber;
        }
        MPI_Bcast(&number[0], 1, MPI_INT, 0, MPI_COMM_WORLD);
        componentsNumber = number[0];
    }
    // if (rank == 0)     printImage(image, height, width);
    return componentsNumber;
}

std::vector<int> combineComponentParts(int ** image, int height, int width, int componentsNumber) {
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int ** graf = new int*[componentsNumber];
    for (int i = 0; i < componentsNumber; i++) {
        graf[i] = new int[componentsNumber];
        for (int j = 0; j < componentsNumber; j++) {
            graf[i][j] = 0;
        }
    }
    for (int i = 1; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (image[i][j] != 0) {
                if (j > 0) {
                    if (image[i - 1][j - 1] != 0 && image[i - 1][j - 1] != image[i][j]) {
                        int g1 = image[i - 1][j - 1] - 1;
                        int g2 = image[i][j] - 1;
                        graf[g1][g2] = 1;
                        graf[g2][g1] = 1;
                    }
                    if (image[i][j - 1] != 0 && image[i][j - 1] != image[i][j]) {
                        int g1 = image[i][j - 1] - 1;
                        int g2 = image[i][j] - 1;
                        graf[g1][g2] = 1;
                        graf[g2][g1] = 1;
                    }
                }
                if (j < width - 1) {
                    if (image[i - 1][j + 1] != 0 && image[i - 1][j + 1] != image[i][j]) {
                        int g1 = image[i - 1][j + 1] - 1;
                        int g2 = image[i][j] - 1;
                        graf[g1][g2] = 1;
                        graf[g2][g1] = 1;
                    }
                    if (image[i][j + 1] != 0 && image[i][j + 1] != image[i][j]) {
                        int g1 = image[i][j + 1] - 1;
                        int g2 = image[i][j] - 1;
                        graf[g1][g2] = 1;
                        graf[g2][g1] = 1;
                    }
                }
                if (image[i - 1][j] != 0 && image[i - 1][j] != image[i][j]) {
                    int g1 = image[i - 1][j] - 1;
                    int g2 = image[i][j] - 1;
                    graf[g1][g2] = 1;
                    graf[g2][g1] = 1;
                }
            }
        }
    }
     //   if (rank == 0)     printImage(graf, componentsNumber, componentsNumber);
    int k;
    std::vector<int>vec;
    for (int i = 0; i < componentsNumber; i++) {
        k = 0;
        vec.clear();
        for (int j = 0; j < componentsNumber; j++) {
            if (graf[i][j] != 0) {
                k++;
                vec.push_back(j);
            }
        }
        if (k > 1) {
            for (unsigned int t = 0; t < vec.size(); t++) {
                for (unsigned int p = 0; p < vec.size(); p++) {
                    graf[vec[t]][vec[p]] = 1;
                }
            }
        }
    }


  //  if (rank == 0)     printImage(graf, componentsNumber, componentsNumber);
    if (size > height) {
        size = height;
    }
    int delta_height = height / size;
    int rem_height = height % size;

    int cNumber = componentsNumber;
    int start_height = (rank < rem_height) ? (rank * (delta_height + 1)) : (delta_height * rank + rem_height);
    int rem_count = (rank < rem_height) ? (delta_height + 1) : delta_height;
    rem_count += start_height;
    for (int i = cNumber - 2; i >= 0; i--) {
        for (int j = cNumber - 1; j >= i + 1; j--) {
            if (graf[i][j] != 0) {
                std::vector<int> send(height*width);
                if (rank >= size) {
                    int k = 0;
                    for (int h = 0; h < height; h++) {
                        for (int w = 0; w < width; w++) {
                            send[k] = cNumber;
                            k++;
                        }
                    }
                } else {
                    for (int h = start_height; h < rem_count; h++) {
                        for (int w = 0; w < width; w++) {
                            if (image[h][w] == j + 1) {
                                image[h][w] = i + 1;
                            }
                        }
                    }
                    int k = 0;
                    for (int h = 0; h < height; h++) {
                        for (int w = 0; w < width; w++) {
                            send[k] = image[h][w];
                            k++;
                        }
                    }
                }
                std::vector<int> recv(height*width);
                MPI_Allreduce(&send[0], &recv[0], height*width, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
                int k = 0;
                for (int h = 0; h < height; h++) {
                    for (int w = 0; w < width; w++) {
                        image[h][w] = recv[k];
                        k++;
                    }
                }
                componentsNumber--;
            }
        }
    }
// if (rank == 0)     printImage(image, height, width);
    std::vector<int> components;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            if (image[h][w] != 0) {
                if (components.empty()) {
                    components.push_back(image[h][w]);
                } else {
                    bool flag = true;
                    int csize = components.size();
                    for (int i = 0; i < csize; i++) {
                        if (components[i] == image[h][w]) {
                            flag = false;
                            break;
                        }
                    }
                    if (flag) {
                        components.push_back(image[h][w]);
                    }
                }
            }
        }
    }

    return components;
}

std::vector<int> searchConnectedComponents(int ** image, int height, int width) {
    int componentsNumber = giveNumbersToComponents(image, height, width);
    std::vector<int> components = combineComponentParts(image, height, width, componentsNumber);
    return components;
}


int ** imageOrAnd(int ** image1, int ** image2, int height, int width, int sign) {
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int delta_height = height / size;
    int rem_height = height % size;
    int start_height = (rank < rem_height) ? (rank * (delta_height + 1)) : (delta_height * rank + rem_height);
    int max_height = (rank < rem_height) ? (delta_height + 1) : delta_height;
    max_height += start_height;

    int ** resultImage = new int*[height];
    for (int i = 0; i < height; i++) {
        resultImage[i] = new int[width];
        for (int j = 0; j < width; j++) {
            resultImage[i][j] = 0;
        }
    }

    for (int i = start_height; i < max_height; i++) {
        for (int j = 0; j < width; j++) {
            if (sign == 1) {  // or
                if (image1[i][j] != 0 || image2[i][j] != 0) {
                    resultImage[i][j] = 1;
                } else {
                    resultImage[i][j] = 0;
                }
            } else {  // and
                if (image1[i][j] != 0 && image2[i][j] != 0) {
                    resultImage[i][j] = 1;
                } else {
                    resultImage[i][j] = 0;
                }
            }
        }
    }

    int k = 0;
    std::vector<int> send(height*width);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            send[k] = resultImage[i][j];
            k++;
        }
    }

    std::vector<int> recv(height*width);
    MPI_Allreduce(&send[0], &recv[0], height*width, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    k = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            resultImage[i][j] = recv[k];
            k++;
        }
    }

    return resultImage;
}

int ** buildConvexHull(int ** image, int height, int width, int op) {
    int i1, j1, i2, j2, i3, j3;
    int min_i, max_i, min_j, max_j;
    switch (op) {
    case 1:
        i1 = 1;
        j1 = -1;
        i2 = 1;
        j2 = 0;
        i3 = 1;
        j3 = 1;
        min_i = 0;
        min_j = -1;
        max_i = 1;
        max_j = 1;
        break;
    case 2:
        i1 = -1;
        j1 = -1;
        i2 = 0;
        j2 = -1;
        i3 = 1;
        j3 = -1;
        min_i = -1;
        min_j = -1;
        max_i = 1;
        max_j = 0;
        break;
    case 3:
        i1 = -1;
        j1 = -1;
        i2 = -1;
        j2 = 0;
        i3 = -1;
        j3 = 1;
        min_i = -1;
        min_j = -1;
        max_i = 0;
        max_j = 1;
        break;
    case 4:
        i1 = -1;
        j1 = 1;
        i2 = 0;
        j2 = 1;
        i3 = 1;
        j3 = 1;
        min_i = -1;
        min_j = 0;
        max_i = 1;
        max_j = 1;
        break;
    case 5:
        i1 = -1;
        j1 = 0;
        i2 = -1;
        j2 = 1;
        i3 = 0;
        j3 = 1;
        min_i = -1;
        min_j = 0;
        max_i = 0;
        max_j = 1;
        break;
    case 6:
        i1 = 0;
        j1 = 1;
        i2 = 1;
        j2 = 1;
        i3 = 1;
        j3 = 0;
        min_i = 0;
        min_j = 0;
        max_i = 1;
        max_j = 1;
        break;
    case 7:
        i1 = 1;
        j1 = 0;
        i2 = 1;
        j2 = -1;
        i3 = 0;
        j3 = -1;
        min_i = 0;
        min_j = -1;
        max_i = 1;
        max_j = 0;
        break;
    case 8:
        i1 = 0;
        j1 = -1;
        i2 = -1;
        j2 = -1;
        i3 = -1;
        j3 = 0;
        min_i = -1;
        min_j = -1;
        max_i = 0;
        max_j = 0;
        break;
    default:
        break;
    }
    int ** resultImage = new int*[height];
    for (int i = 0; i < height; i++) {
        resultImage[i] = new int[width];
        for (int j = 0; j < width; j++) {
            resultImage[i][j] = image[i][j];
        }
    }
    int k = 1;
    while (k != 0) {
        k = 0;
        for (int i = 0 - min_i; i < height - max_i; i++) {
            for (int j = 0 - min_j; j < width - max_j; j++) {
                if (resultImage[i][j] == 0) {
                    if (resultImage[i + i1][j + j1] != 0 &&
                        resultImage[i + i2][j + j2] != 0 &&
                        resultImage[i + i3][j + j3] != 0) {
                        k++;
                        resultImage[i][j] = 1;
                    }
                }
            }
        }
    }
    return resultImage;
}

int ** buildComponentConvexHull(int ** image, int height, int width) {
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // buildConvexHull(image, height, width, 8);

    int ** image1 = new int*[height];
    int ** image2 = new int*[height];
    for (int i = 0; i < height; i++) {
        image1[i] = new int[width];
        image2[i] = new int[width];
        for (int j = 0; j < width; j++) {
            image1[i][j] = image[i][j];
            image2[i][j] = image[i][j];
        }
    }
    int ** resultImage;
    image1 = imageOrAnd(imageOrAnd(buildConvexHull(image1, height, width, 1),
                             buildConvexHull(image1, height, width, 2),
                             height, width, 1),
                     imageOrAnd(buildConvexHull(image1, height, width, 3),
                             buildConvexHull(image1, height, width, 4),
                             height, width, 1),
                     height, width, 1);
    image2 = imageOrAnd(imageOrAnd(buildConvexHull(image2, height, width, 5),
                             buildConvexHull(image2, height, width, 6),
                             height, width, 1),
                     imageOrAnd(buildConvexHull(image2, height, width, 7),
                             buildConvexHull(image2, height, width, 8),
                             height, width, 1),
                     height, width, 1);
    // if (rank == 0) {
    //     printImage(image1, height, width);
    //     std::cout << "--"<<std::endl;
    //     printImage(image2, height, width);
    // }
    resultImage = imageOrAnd(image1, image2, height, width, 0);
    return resultImage;
}

int ** getComponent(int ** image, int height, int width, int number) {
    int ** resultImage = new int*[height];
    for (int i = 0; i < height; i++) {
        resultImage[i] = new int[width];
        for (int j = 0; j < width; j++) {
            if (image[i][j] == number) {
                resultImage[i][j] = 1;
            } else {
                resultImage[i][j] = 0;
            }
        }
    }
    return resultImage;
}

int ** buildImageConvexHull(int ** image, int height, int width) {
    int ** resultImage = new int*[height];
    int ** comp = new int*[height];
    for (int i = 0; i < height; i++) {
        resultImage[i] = new int[width];
        comp[i] = new int[width];
        for (int j = 0; j < width; j++) {
            resultImage[i][j] = 0;
            comp[i][j] = image[i][j];
        }
    }
    std::vector<int> components = searchConnectedComponents(comp, height, width);

    while (!components.empty()) {
        int component = components.back();
        components.pop_back();
        int ** result = buildComponentConvexHull(getComponent(comp, height, width, component), height, width);
        resultImage = imageOrAnd(resultImage, result, height, width, 1);
    }
    return resultImage;
}
