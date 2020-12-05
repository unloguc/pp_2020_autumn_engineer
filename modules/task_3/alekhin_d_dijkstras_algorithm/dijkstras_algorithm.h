// Copyright 2020 Alekhin Denis
#ifndef MODULES_TASK_3_ALEKHIN_D_DIJKSTRAS_ALGORITHM_DIJKSTRAS_ALGORITHM_H_
#define MODULES_TASK_3_ALEKHIN_D_DIJKSTRAS_ALGORITHM_DIJKSTRAS_ALGORITHM_H_

#include <vector>

void getRandomGraph(std::vector<int>* graph, int size = 5);

void printGraph(const std::vector<int>* graph);

void printResult(const std::vector<int>* result);

std::vector<int> getDijkstrasAlgorithmSequential(const std::vector<int>* graph, int root);

std::vector<int> getDijkstrasAlgorithmParallel(const std::vector<int>* graph, int root);

#endif  // MODULES_TASK_3_ALEKHIN_D_DIJKSTRAS_ALGORITHM_DIJKSTRAS_ALGORITHM_H_
