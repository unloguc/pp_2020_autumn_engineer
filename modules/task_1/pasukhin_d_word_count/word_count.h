// Copyright 2020 Pasukhin Dmitry

#ifndef MODULES_TASK_1_PASUKHIN_D_WORD_COUNT_WORD_COUNT_H_
#define MODULES_TASK_1_PASUKHIN_D_WORD_COUNT_WORD_COUNT_H_

#include <string>
#include <vector>

std::string getRandomString(const int size);
int getParallelOperations(const std::string global_string, const int size_string);
int getSequentialOperations(const std::string input_string, const int size, int *s_first_letter, int *is_last_letter);

#endif  // MODULES_TASK_1_PASUKHIN_D_WORD_COUNT_WORD_COUNT_H_
