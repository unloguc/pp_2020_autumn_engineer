// Copyright 2020 Anisimova Polina
#include <mpi.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <utility>
#include "../../../modules/task_3/anisimova_p_global_search/anisimova_p_agp.h"

dpair seqGlobSearch(const std::function<double(double)>& func, double a, double b, double eps, double r) {
    std::vector<dpair> vec;    //вектор, в котором хранятся все результаты испытаний
    vec.push_back(dpair(a, func(a)));
    vec.push_back(dpair(b, func(b)));
    double M = 0;
    size_t k = 2;  // Число точек в векторе
    size_t t = 0;

    //константа Липшица
    for (; (vec[t + 1].first - vec[t].first) > eps; ++k) {
        for (size_t i = 0; i < (k - 1u); ++i) {
            double M_tmp = std::abs((vec[i + 1].second - vec[i].second) / (vec[i + 1].first - vec[i].first));
            if (M_tmp > M)
                M = M_tmp;
        }

        double m = 1.0;
        if (M != 0.0)
            m = r * M;


        t = 0;  //Характеристика R
        double R = m * (vec[1].first - vec[0].first) + (vec[1].second - vec[0].second) *
        (vec[1].second - vec[0].second) / (m * (vec[1].first - vec[0].first)) -
        2 * (vec[1].second + vec[0].second);
        for (size_t i = 1; i < (k - 1u); ++i) {
            double R_tmp = m * (vec[i + 1u].first - vec[i].first) + (vec[i + 1u].second - vec[i].second) *
            (vec[i + 1u].second - vec[i].second) / (m * (vec[i + 1u].first - vec[i].first)) -
            2 * (vec[i + 1u].second + vec[i].second);
            if (R_tmp > R) {
                t = i;
                R = R_tmp;
            }
        }

        double x_t1 = (vec[t].first + vec[t + 1].first) / 2 - (vec[t + 1].second - vec[t].second) / (2 * m);
        dpair t1_pair = dpair(x_t1, func(x_t1));
        vec.insert(std::lower_bound(vec.begin(), vec.end(), t1_pair,
            [] (const dpair& a, const dpair& b) {
            return a.first <= b.first;
            }), t1_pair);
    }
    return vec[t + 1];
}

dpair parGlobSearch(const std::function<double(double)>& func, double a, double b, double eps, double r) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::vector<dpair> vec;  // Вектор, в котором хранятся результаты испытаний
        vec.push_back(dpair(a, func(a)));
        vec.push_back(dpair(b, func(b)));
        double M = 0.0;
        int k = 2;  // Число точек в векторе
        std::vector<std::pair<int, double> > tj_vec;
        for (; true; k = static_cast<int>(vec.size())) {
            int tj_size = (static_cast<int>(tj_vec.size()) < size) ? static_cast<int>(tj_vec.size()) : size;
            for (int i = 0; i < (k - 1); ++i) {
                if ((vec[i + 1].first - vec[i].first) < eps) {  //искать только по интервалам
                    int stop = 1;
                    for (int i = 1; i < size; ++i) {  //рассылка сигналов остановки для процессов с рангом !=0
                        MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    }
                    dpair min = vec[0];
                    for (int j = 1; j < k; ++j) {  //поиск минимального значения
                        if (vec[j].second < min.second)
                            min = vec[j];
                    }
                    return min;
                }
            }
            for (int i = 0; i < (k - 1); ++i) {  //константа Липшица
                double M_tmp = std::abs((vec[i + 1].second - vec[i].second) / (vec[i + 1].first - vec[i].first));
                if (M_tmp > M)
                    M = M_tmp;
            }

            double m = 1.0;
            if (M != 0.0)
                m = r * M;
            tj_vec.clear();
            double R;
            for (int i = 0; i < (k - 1); ++i) {
                R = m * (vec[i + 1].first - vec[i].first) + (vec[i + 1].second - vec[i].second) *
                    (vec[i + 1].second - vec[i].second) / (m * (vec[i + 1].first - vec[i].first)) -
                    2 * (vec[i + 1].second + vec[i].second);
                tj_vec.push_back(dpair(i, R));
            }
            tj_size = (static_cast<int>(tj_vec.size()) < size) ? static_cast<int>(tj_vec.size()) : size;
            for (int j = 0; j < tj_size; ++j) {  // упорядочивание характеристик по убыванию
                for (int l = j + 1; l < static_cast<int>(tj_vec.size()); ++l) {
                    if (tj_vec[l].second > tj_vec[j].second) {
                        std::swap(tj_vec[l], tj_vec[j]);
                    }
                }
            }
            double x_t0 = (vec[tj_vec[0].first + 1].first + vec[tj_vec[0].first].first) / 2 -
                (vec[tj_vec[0].first + 1].second - vec[tj_vec[0].first].second) / (2 * m);
            std::vector<dpair> tmp_vec(tj_size);
            tmp_vec[0] = dpair(x_t0, 0.0);
            for (int i = 1; i < tj_size; ++i) {
                double x_t = (vec[tj_vec[i].first + 1].first + vec[tj_vec[i].first].first) / 2 -
                    (vec[tj_vec[i].first + 1].second - vec[tj_vec[i].first].second) / (2 * m);
                int stop = 0;
                MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&x_t, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
                tmp_vec[i]=(dpair(x_t, 0.0));
            }
            tmp_vec[0].second = func(x_t0);
            for (int i = 1; i < tj_size; ++i) {
                MPI_Status status;
                MPI_Recv(&tmp_vec[i].second, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
            }
            for (auto & t_pair : tmp_vec) {
                vec.insert(std::lower_bound(vec.begin(), vec.end(), t_pair,
                    [](const dpair& a, const dpair& b) {
                        return a.first <= b.first;
                    }), t_pair);
            }
        }
    } else {
        double x;
        int stop = 0;
        MPI_Status status;
        while (stop == 0) {
            MPI_Recv(&stop, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            if (stop != 0)
                return dpair();
            MPI_Recv(&x, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
            double y = func(x);
            MPI_Send(&y, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }
    return dpair();
}
