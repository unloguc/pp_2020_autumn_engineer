// Copyright 2020 Kustova Anastasiya
#ifndef MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_
#define MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_

#include <mpi.h>
#include <vector>

void printImage(int ** image, int height, int width);
std::vector<int> searchConnectedComponents(int ** image, int height, int width);
int ** buildComponentConvexHull(int ** image, int height, int width);
int ** buildImageConvexHull(int ** image, int height, int width);
#endif  // MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_
