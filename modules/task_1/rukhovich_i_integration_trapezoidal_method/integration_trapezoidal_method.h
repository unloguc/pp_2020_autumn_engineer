#ifndef MODULES_TASK_1_RUKHOVICH_I_INTEGRATION_TRAPEZOIDAL_METHOD_
#define MODULES_TASK_1_RUKHOVICH_I_INTEGRATION_TRAPEZOIDAL_METHOD_

#include <shared_ptr>

class Function {
public:
    Function() = default;

    virtual ~Function() = 0;
    virtual double operator()(double) = 0;
}

class QuadraticPolynomial : public Function{
    double a, b, c;
public:
    QuadraticPolynomial(double, double, double);

    virtual ~QuadraticPolynomial();
    virtual double operator()(double);
}

double getIntegralSequentional(std::shared_ptr<Function>, double, double, double);

double getIntegralParallel(std::shared_ptr<Function>, double, double, double);

#endif  // MODULES_TASK_1_RUKHOVICH_I_INTEGRATION_TRAPEZOIDAL_METHOD_
