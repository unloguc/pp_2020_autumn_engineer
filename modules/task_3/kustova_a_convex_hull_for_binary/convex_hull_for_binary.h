// Copyright 2020 Kustova Anastasiya
#ifndef MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_
#define MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_

#include <mpi.h>
#include <vector>
struct Point {
    int x, y;
    friend Point operator-(const Point& a, const Point& b) {
        return Point{a.x - b.x, a.y - b.y};
    }
    friend bool operator!=(const Point& a, const Point& b) {
        return !(a == b);
    }
    friend bool operator==(const Point& a, const Point& b) {
        return (a.x == b.x) && (a.y == b.y);
    }
};
void printImage(int ** image, int height, int width);
std::vector<int> searchConnectedComponents(int ** image, int height, int width);
int ** buildComponentConvexHull(int ** image, int height, int width, int type);
int ** buildImageConvexHull(int ** image, int height, int width, int type);
std::vector<Point> buildHull(std::vector<Point> m_set);
std::vector<Point> buildHull_sequential(std::vector<Point> m_set);
Point find_leftmost_point(std::vector<Point> buf, int rank, int size, int countpr);
bool is_counter_clockwise(const Point& p, const Point& i, const Point& q);
int ** getComponent(int ** image, int height, int width, int number);
int ** imageOr(int ** image1, int ** image2, int height, int width);
std::vector<int> combineComponentParts(int ** image, int height, int width, int componentsNumber);
int giveNumbersToComponents(int ** image, int height, int width);
void printImage(int ** image, int height, int width);
#endif  // MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_
