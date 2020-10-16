#ifndef MODULES_TASK_1_INTEGRATION_H_
#define MODULES_TASK_1_INTEGRATION_H_

#include <functional>
#include <math.h>
#include<vector>

double myFunc(double);

extern  std::function<double(double)> f;

// passing function, number of segments and integration limits
double localIntegration(double*, double*, int);

// passing function, number of segments and integration limits
double integration(std::function<double(double)>,int, int, int);

#endif // MODULES_TASK_1_INTEGRATION_H_