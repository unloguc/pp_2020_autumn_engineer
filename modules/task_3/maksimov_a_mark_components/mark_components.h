// Copyright 2020 Maksimov Andrey
#ifndef MODULES_TASK_3_MAKSIMOV_A_MARK_COMPONENTS_MARK_COMPONENTS_H_
#define MODULES_TASK_3_MAKSIMOV_A_MARK_COMPONENTS_MARK_COMPONENTS_H_

#include <vector>

std::vector<uint32_t> getRandomBinaryImage(int height, int width);
std::vector<uint32_t> markComponentsNotParall(std::vector<uint32_t> image, int height, int width);
std::vector<uint32_t> markComponents(std::vector<uint32_t> image, int height, int width);

#endif  // MODULES_TASK_3_MAKSIMOV_A_MARK_COMPONENTS_MARK_COMPONENTS_H_
