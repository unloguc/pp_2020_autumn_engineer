#include <mpi.h>
#include <stdexcept>
#include <iostream>
#include "./integration_trapezoidal_method.h"


QuadraticPolynomial::QuadraticPolynomial(double a, double b, double c) : a(a), b(b), c(c) {}

double QuadraticPolynomial::operator()(double point) {
    return a*point*point + b*point + c;
}

double getIntegralSequentional(std::shared_ptr<Function> func, double from, double to,
                               double precision) {
    if (to < from) {
        throw std::runtime_error("'from' must not be greater than 'to'");
    }
    if (precision <= 0.) {
        throw std::runtime_error("'precision' must be greater than 0");
    }

    double cur_val = (*func)(from), cur_point = from, ans = 0.;
    if (cur_val < 0.) {
        throw std::runtime_error("Function value must not be less than 0");
    }
    while (cur_point + precision < to) {
        cur_point += precision;

        double next_val = (*func)(cur_point);
        if (next_val < 0.) {
            throw std::runtime_error("Function value must not be less than 0");
        }
        ans += (cur_val + next_val) * precision / 2.;

        cur_val = next_val;
    }

    double last_val = (*func)(to);
    if (last_val < 0.) {
        throw std::runtime_error("Function value must not be less than 0");
    }
    ans += (cur_val + last_val) * (to - cur_point) / 2.;
    return ans;
}

double getIntegralParallel(std::shared_ptr<Function> func, double from, double to,
                           double precision) {
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double delta = (to - from) / double(size);
    if (rank == 0) {
        if (to < from) {
            throw std::runtime_error("'from' must not be greater than 'to'");
        }
        if (precision <= 0.) {
            throw std::runtime_error("'precision' must be greater than 0");
        }
    }
    from += rank * delta;
    // std::cout << "OK from " << rank << ", itervals are " << from << ' ' << from + delta << '\n';
    double local_result = getIntegralSequentional(func, from, from + delta, precision);
    // std::cout << "OK calc from " << rank << ", result is " << local_result << '\n';
    double ans = 0;
    MPI_Reduce(&local_result, &ans, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    // std::cout << "OK from" << rank << ", ans is " << ans << '\n';
    return ans;
}
