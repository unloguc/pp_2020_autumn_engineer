// Copyright 2020 Khismatulina Karina
#ifndef MODULES_TASK_2_KHISMATULINA_K_CONTRAST_RAISE_CONTRAST_RAISE_H_
#define MODULES_TASK_2_KHISMATULINA_K_CONTRAST_RAISE_CONTRAST_RAISE_H_

#include <vector>

std::vector<int> getRandomImage(int size);
int getMidBright(std::vector<int> imageData, int size);
std::vector<int> contrastRaiseSeq(std::vector<int> imageData, int size, int contrast, int midBright);
std::vector<int> contrastRaiseParallel(std::vector<int> imageData, int size, int contrast);

#endif  // MODULES_TASK_2_KHISMATULINA_K_CONTRAST_RAISE_CONTRAST_RAISE_H_
