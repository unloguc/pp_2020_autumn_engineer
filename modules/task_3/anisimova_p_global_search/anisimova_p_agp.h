// Copyright 2020 Anisimova Polina

#ifndef MODULES_TASK_3_ANISIMOVA_P_GLOBAL_SEARCH_ANISIMOVA_P_AGP_H_
#define MODULES_TASK_3_ANISIMOVA_P_GLOBAL_SEARCH_ANISIMOVA_P_AGP_H_

#include <functional>
#include <utility>

typedef std::pair<double, double> dpair;
//последовательный метод
dpair seqGlobSearch(const std::function<double(double)>& func, double a, double b,
    double eps = 0.0001, double r = 2.0);
//параллельный метод
dpair parGlobSearch(const std::function<double(double)>& func, double a, double b,
    double eps = 0.0001, double r = 2.0);

#endif  // MODULES_TASK_3_ANISIMOVA_P_GLOBAL_SEARCH_ANISIMOVA_P_AGP_H_
