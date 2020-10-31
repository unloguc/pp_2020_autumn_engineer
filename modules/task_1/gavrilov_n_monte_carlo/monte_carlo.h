// Copyright 2020 Gavrilov Nikita
#ifndef MODULES_TASK_1_GAVRILOV_MONTE_CARLO_INTEGRAL_MONTE_CARLO_H
#define MODULES_TASK_1_GAVRILOV_MONTE_CARLO_INTEGRAL_MONTE_CARLO_H

double getIntegral(double _min, double _max, int splitCount, double(*func)(double));
double getIntegralParallel(double _min, double _max, int splitCount, double(*func)(double));

#endif  // MODULES_TASK_1_GAVRILOV_MONTE_CARLO_INTEGRAL_MONTE_CARLO_H
