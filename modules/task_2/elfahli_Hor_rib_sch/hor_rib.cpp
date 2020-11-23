// Copyright 2020 MERYEM EL FAHLI
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <ctime>
#include <vector>
#include <random>
#include <algorithm>
#include "../../../modules/task_2/elfahli_Hor_rib_sch/hor_rib.h"
using namespace std;
vector<double> seq(vector < double > M, vector < double > V, int l, int c) {
    vector<double> vec(c);
    for (int i = 0; i < c; i++) {
        vec[i] = r(i, M,V,l);
    }
    return vec;
}
double r(int i, vector<double> M,vector<double> v, int l) {
    double s = 0;
    for (int j = 0; j < l; j++) {
        s = s + M[j + i * l] * v[j];
    }
    return s;
}
vector<double> par(vector < double > M, vector < double > V, int l, int c) {
    int s, rk;
    MPI_Comm_size(MPI_COMM_WORLD, &s);
    MPI_Comm_rank(MPI_COMM_WORLD, &rk);
    int count = c / s;
    int rf = c % s;
    vector<double> l_m(count * l);
    vector<double> l_v(l);
    MPI_Status Status;
    if (rk == 0) {
        for (int i = 0; i < count * l; i++) {
            l_m[i] = M[i];
        }
        for (int i = 0; i < l; i++) {
            l_v[i] = V[i];
        }
        for (int i = 1; i < s; i++) {
            MPI_Send(M.data() + i * count * l, count * l, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(l_v.data(), l, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(l_m.data(), count * l, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &Status);
        MPI_Recv(l_v.data(), l, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &Status);
    }
    vector<double> mult = seq(l_m, l_v, l, count);
    vector<double> rt_mult(c);
    if (rk == 0) {
        for (int i = 0; i < count; i++)
            rt_mult[i] = mult[i];
        for (int i = 1; i < s; i++) {
            MPI_Recv(rt_mult.data() + i * count, count, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &Status);
        }
    } else {
        MPI_Send(mult.data(), count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    if (rk == 0) {
        if (rf != 0) {
            for (int j = c - rf; j < c; j++)
                rt_mult[j] = r(j, M, V, l);
        }
    }
    return rt_mult;
}
vector<double> gen_mat(int l, int c) {
    mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    vector<double> list(c * l);
    for (int i = 0; i < c * l; i++)
        list[i] = gen() % 10;
    return list;
}
vector<double> gen_vec(int n) {
    mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<double> v(n);
    for (int i = 0; i < n; i++)
        v[i] = gen() % 10;
    return v;
}