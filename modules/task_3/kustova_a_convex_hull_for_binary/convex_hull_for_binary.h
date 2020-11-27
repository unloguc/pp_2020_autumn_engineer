// Copyright 2020 Kustova Anastasiya
#ifndef MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_
#define MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_

#include <mpi.h>
#include <vector>
struct Point {
  int x, y;

  friend Point operator+(const Point& a, const Point& b);
  friend Point operator-(const Point& a, const Point& b) {
    return Point{a.x - b.x, a.y - b.y};
  }
  friend bool operator!=(const Point& a, const Point& b) {
    return !(a == b);
  }
  friend bool operator==(const Point& a, const Point& b) {
    return (a.x == b.x) && (a.y == b.y);
  }

  friend bool operator<(const Point& a, const Point& b) {
    return a.x < b.x;
  }
};
void printImage(int ** image, int height, int width);
std::vector<int> searchConnectedComponents(int ** image, int height, int width);
int ** buildComponentConvexHull(int ** image, int height, int width);
int ** buildImageConvexHull(int ** image, int height, int width);
std::vector<Point> buildHull(std::vector<Point> m_set);
std::vector<Point> buildHull_s(std::vector<Point> m_set);
double my_abs(double x);
#endif  // MODULES_TASK_3_KUSTOVA_A_CONVEX_HULL_FOR_BINARY_CONVEX_HULL_FOR_BINARY_H_
