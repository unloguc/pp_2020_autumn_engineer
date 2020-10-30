#include <mpi.h>
#include <stdexcept>
#include "./integration_trapezoidal_method.h"


QuadraticPolynomial::QuadraticPolynomial(double a, double b, double c) : a(a), b(b), c(c) {}

virtual QuadraticPolynomial::~QuadraticPolynomial() {}

virtual double QuadraticPolynomial::operator()(double point) {
    return a*point*point + b*point + c;
}

double getIntegralSequentional(std::shared_ptr<Function> func, double from, double to,
                               double precision) {
    if (to < from) {
        throw std::runtime_error("'from' must not be greater than 'to'");
    }
    if (precision <= 0d) {
        throw std::runtime_error("'precision' must be greater than 0");
    }

    double cur_val = func(from), cur_point = from, ans = 0d;
    if (cur_val < 0) {
        throw std::runtime_error("Function value must not be less than 0");
    }
    while (cur_point + precision < to) {
        cur_point += precision;

        double next_val = func(cur_point);
        if (next_val < 0) {
            throw std::runtime_error("Function value must not be less than 0");
        }
        ans += (cur_val + next_val) * precision / 2d;

        cur_val = next_val;
    }

    double last_val = func(to);
    if (last_val < 0) {
        throw std::runtime_error("Function value must not be less than 0");
    }
    ans += (cur_val + last_val) * (to - cur_point) / 2d;
    return ans;
}

double getIntegralParallel(std::shared_ptr<Function> func, double from, double to,
                           double precision) {
    throw std::runtime_error("Not implemented yet");
}
/*
int getParallelOperations(std::vector<int> global_vec,
                          int count_size_vector, std::string ops) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int delta = count_size_vector / size;

    if (rank == 0) {
        for (int proc = 1; proc < size; proc++) {
            MPI_Send(&global_vec[0] + proc * delta, delta,
                        MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    }

    std::vector<int> local_vec(delta);
    if (rank == 0) {
        local_vec = std::vector<int>(global_vec.begin(),
                                     global_vec.begin() + delta);
    } else {
        MPI_Status status;
        MPI_Recv(&local_vec[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int global_sum = 0;
    int local_sum = getSequentialOperations(local_vec, ops);
    MPI_Op op_code;
    if (ops == "+" || ops == "-") { op_code = MPI_SUM; }
    if (ops == "max") { op_code = MPI_MAX; }
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return global_sum;
}
*/