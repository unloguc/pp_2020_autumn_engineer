#ifndef MODULES_TASK_1_RUKHOVICH_I_INTEGRATION_TRAPEZOIDAL_METHOD_
#define MODULES_TASK_1_RUKHOVICH_I_INTEGRATION_TRAPEZOIDAL_METHOD_

#include <vector>
#include <string>

std::vector<int> getRandomVector(int  sz);
int getParallelOperations(std::vector<int> global_vec,
                          int count_size_vector, std::string ops);
int getSequentialOperations(std::vector<int> vec, std::string ops);

#endif  // MODULES_TASK_1_RUKHOVICH_I_INTEGRATION_TRAPEZOIDAL_METHOD_
