// Copyright 2020 Kustova Anastasiya
#include "../../../modules/task_3/kustova_a_convex_hull_for_binary/convex_hull_for_binary.h"
#include <mpi.h>
#include <vector>
#include <random>
#include <iostream>
#include <ctime>
void printImage(int ** image, int height, int width) {
    // for (int i = 0; i < height; i++) {
    //     for (int j = 0; j < width; j++) {
    //         std::cout << image[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }
        for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (image[i][j] == 0) {
            std::cout << "  ";
            } else if (image[i][j] == 1) {
            std::cout << "w ";
            } else {
                std::cout << 3 << " ";
            }
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
        int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int componentsNumber = giveNumbersToComponents(image, height, width);
    std::vector<int> components = combineComponentParts(image, height, width, componentsNumber);
    return components;
}


int ** imageOr(int ** image1, int ** image2, int height, int width) {
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
            if (image1[i][j] != 0) {
                resultImage[i][j] = image1[i][j];
            } else if (image2[i][j] != 0) {
                resultImage[i][j] = image2[i][j];
            } else {
                resultImage[i][j] = 0;
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


int ** buildComponentConvexHull(int ** image, int height, int width) {
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    std::vector<Point> vec;
    std::vector<Point> res;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (image[i][j] != 0) {
                Point a;
                a.x = i;
                a.y = j;
            vec.push_back(a);
            }
        }
    }
    res = buildHull(vec);
    for (unsigned int i = 0; i < res.size(); i++) {
        int x1 = static_cast<int>(res[i].x);
        int y1 = static_cast<int>(res[i].y);
        image[x1][y1] = 3;
    }
    return image;
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
         int rank;
     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
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
        resultImage = imageOr(resultImage, result, height, width);
    }
    return resultImage;
}

int cross(const Point& a, const Point& b) {
  return (a.x * b.y) - (b.x * a.y);
}

bool is_counter_clockwise(const Point& p, const Point& i, const Point& q) {
  return cross(i - p, q - p) < 0;
}

Point find_leftmost_point(std::vector<Point> buf, int rank, int size, int countpr) {
    Point most_point;
    if (buf.size() != 0) {
        most_point = buf[0];
    }
    for (unsigned int i = 1; i < buf.size(); i++) {
        if (buf[i].x < most_point.x) {
            most_point = buf[i];
        }
    }

    std::vector<Point> points(size);
    if (countpr == 0) {
        MPI_Gather(&most_point, 2, MPI_INT, &points[0], 2, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        most_point = points[0];
        for (unsigned int i = 0; i < points.size(); i++) {
            if (points[i].x == 0) continue;
            else
            if (points[i].x < most_point.x) {
                most_point = points[i];
            }
        }
    }
    MPI_Bcast(&most_point, 2, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    return most_point;
}

std::vector<Point> buildHull(std::vector<Point> m_set) {
    int rank, size;
    int countpr = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    Point leftmost_point;
    int delta = m_set.size() / size;
    int rem = m_set.size() % size;
    if (delta == 0) countpr = 1;
    std::vector<Point> buffer(delta);
    MPI_Scatter(&m_set[rem], delta * 2, MPI_INT, &buffer[0], delta * 2, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        buffer.insert(buffer.begin(), m_set.begin(), m_set.begin() + rem);
    }
    leftmost_point = find_leftmost_point(buffer, rank, size, countpr);
    std::vector<Point> hull;
    std::vector<Point> points(size);
    Point endpoint;
    do {
        hull.push_back(leftmost_point);
        endpoint = m_set.at(0);
        if (countpr == 0 || rank == 0) {
            for (auto i = buffer.begin(); i < buffer.end(); i++) {
                if (is_counter_clockwise(hull.back(), *i, endpoint) || endpoint == leftmost_point) {
                    endpoint = *i;
                }
            }
        }
        if (countpr == 0) {
            MPI_Gather(&endpoint, 2, MPI_INT, &points[0], 2, MPI_INT, 0, MPI_COMM_WORLD);
        }
        if (rank == 0) {
            for (auto point : points) {
                if (is_counter_clockwise(hull.back(), point, endpoint) || endpoint == leftmost_point) {
                    endpoint = point;
                }
            }
        }
        MPI_Bcast(&endpoint, 2, MPI_INT, 0, MPI_COMM_WORLD);
        leftmost_point = endpoint;
    } while (endpoint != hull.at(0));
    return hull;
}

// std::vector<Point> buildHull_s(std::vector<Point> m_set) {
//   Point leftmost_point = m_set[0];
//   for (unsigned int i = 1; i < m_set.size(); i++) {
//     if (m_set[i].x < leftmost_point.x) {
//         leftmost_point = m_set[i];
//     }
//   }
//   std::vector<Point> hull;
//   Point endpoint;
//   do {
//     hull.push_back(leftmost_point);
//     endpoint = m_set.at(0);
//     for (auto i : m_set) {
//       if (is_counter_clockwise(hull.back(), i, endpoint) ||
//           endpoint == leftmost_point) {
//         endpoint = i;
//       }
//     }

//     leftmost_point = endpoint;
//   } while (endpoint != hull.at(0));

//   return hull;
// }
