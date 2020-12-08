// Copyright 2020 Kochankov Ilya
#include "../../../modules/task_3/kochankov_i_sort_double_simple_merge/bitwise_sort_for_double_with_simple_merge.h"
#include <mpi.h>
#include <math.h>
#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <vector>
#include <utility>

using std::vector;
using std::cout;
using std::flush;
using std::endl;
using std::find;
using std::copy;
using std::to_string;
using std::pair;
using std::make_pair;

std::vector<double> getRandomVector(int size) {
    std::vector<double> vec(size);
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < size; i++)
        vec[i] = gen() / 10000;
    return vec;
}

std::vector<double> linear_bitwise_sort(const std::vector<double>& vect) {
    double max_number = vect[std::distance(vect.begin(), max_element(vect.begin(), vect.end()))];
    int digits_above_zero = get_digit_number_above_zero(max_number);
    int max_digits_below_zero = 0;
    int digits_below_zero;

    for (auto element : vect) {
        digits_below_zero = get_digit_number_below_zero(element);
        if (digits_below_zero > max_digits_below_zero) {
            max_digits_below_zero = digits_below_zero;
        }
    }

    vector<double> result(static_cast<int>(vect.size()));
    copy(vect.begin(), vect.end(), result.begin());

    for (int i = -(max_digits_below_zero); i <= digits_above_zero; i++) {
        result = bitwise_sort(result, i);
    }

    return result;
}

std::vector<double> bitwise_sort(const std::vector<double>& vect, int discharge) {
    vector<vector<double>> baskets(10);

    fill(baskets.begin(), baskets.end(), vector<double>(0));

    for (auto element : vect) {
        baskets[get_digit(element, discharge)].push_back(element);
    }

    vector<double> result;
    for (auto basket : baskets) {
        for (auto element : basket) {
            result.push_back(element);
        }
    }

    return result;
}

int get_digit(double number, int discharge) {
    if (discharge < 0) {
        return uint64_t(number * pow(10, -discharge)) % 10;
    }
    double mask = pow(10, discharge);
    double tmp = number / mask;
    return static_cast<int>(tmp) % 10;
}

int get_digit_number_above_zero(int number) {
    int digit = 0;
    while (number > 0) {
        number = number / 10;
        digit++;
    }
    return digit;
}

int get_digit_number_below_zero(double number) {
    auto s = to_string(number);
    if (s.find('.')) {
        return -(static_cast<int>(s.find('.')) - static_cast<int>(s.size())) - 1;
    } else {
        return 0;
    }
}

std::vector<double> parallel_bitwise_sort(const std::vector<double>& vect) {
    int size, rank, len = static_cast<int>(vect.size());

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if ((len < size) || (size == 1)) {
        if (rank == 0) {
            return linear_bitwise_sort(vect);
        } else {
            return vector<double>();
        }
    }
    // cout << "Process " << rank << " enter parallel zone" << std::flush << endl;

    int delta = len / size;
    int start_index;
    vector<double> local_vect;

    // cout << "Process " << rank << " continued parallel zone" << endl;
    int count = delta;
    if (rank == 0) {
        local_vect = vector<double>(count);
        std::copy(vect.begin(), vect.begin() + delta, local_vect.begin());

        for (int process_num = 1; process_num < size; process_num++) {
            start_index = delta * process_num;
            if (process_num == size - 1) {
                count = len - start_index;
            }
            MPI_Send(&vect[0] + delta * process_num, count, MPI_DOUBLE, process_num, 0, MPI_COMM_WORLD);
        }
        count = delta;
    } else {
        MPI_Status status;
        if (rank == size - 1) {
            count = len - delta * (size - 1);
        }
        // cout << "Process " << rank << " got " << count << " rows" << std::flush << endl;

        local_vect = vector<double>(count);
        MPI_Recv(&local_vect[0], count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    }

    auto result = linear_bitwise_sort(local_vect);

    // copy(result.begin(), result.end(), ostream_iterator<double>(cout, " "));
    // cout << flush << endl;

    vector<pair<int, int> > process_num_map(size);
    for (int i = 0; i < size - 1; i++) {
        process_num_map[i] = make_pair(i, delta);
    }
    process_num_map[size - 1] = make_pair(size - 1, len - delta * (size - 1));

    while (static_cast<int>(process_num_map.size()) > 1) {
        int index = std::find(process_num_map.begin(),
            process_num_map.end(), make_pair(rank, count)) - process_num_map.begin();

        if ((static_cast<int>(process_num_map.size()) % 2) &&
            (index == static_cast<int>(process_num_map.size()) - 1)) {
            vector<pair<int, int> > tmp_proc;
            for (int i = 1; i < static_cast<int>(process_num_map.size()); i += 2) {
                tmp_proc.push_back(process_num_map[i]);
            }
            tmp_proc.push_back(process_num_map[index]);

            for (int i = 0; i < static_cast<int>(tmp_proc.size()); i++) {
                if (tmp_proc[i].first != rank) {
                    tmp_proc[i].second = tmp_proc[i].second + tmp_proc[i].second;
                }
            }
            process_num_map = tmp_proc;
            continue;
        }

        // cout << "Process " << rank << " has " << process_num_map.size() << "active process "<< flush << endl;

        if (index % 2 == 0) {
            // cout << "Process " << rank << " ready to send the message: "<< flush << endl;

            MPI_Send(&result[0], count, MPI_DOUBLE, process_num_map[index + 1].first, 0, MPI_COMM_WORLD);
            // cout << "Process " << rank << " sent the message: "<< flush << endl;
            if (rank != 0) {
                return vector<double>();
            } else {
                process_num_map = vector<pair<int, int> >(1);
                process_num_map[0] = make_pair(size - 1, static_cast<int>(vect.size()));
            }
        } else {
            MPI_Status status;

            vector<double> tmp(process_num_map[index - 1].second);
            MPI_Recv(&tmp[0], process_num_map[index - 1].second, MPI_DOUBLE,
                process_num_map[index - 1].first, 0, MPI_COMM_WORLD, &status);

            result = merge(result, tmp);
            count = static_cast<int>(result.size());

            // cout << "Process " << rank << " merged: "<< flush << endl;
            // copy(result.begin(), result.end(), ostream_iterator<double>(cout, " "));
            // cout << flush << endl;

            // cout << "Process " << rank << " reviceved the message: "<< flush << endl;


            vector<pair<int, int> > tmp_proc;
            for (int i = 1; i < static_cast<int>(process_num_map.size()); i += 2) {
                tmp_proc.push_back(process_num_map[i]);
            }
            if ((static_cast<int>(process_num_map.size()) != 1) &&
                (static_cast<int>(process_num_map.size()) % 2)) {
                tmp_proc.push_back(process_num_map[static_cast<int>(process_num_map.size()) - 1]);
            }

            for (int i = 0; i < static_cast<int>(tmp_proc.size()); i++) {
                if (tmp_proc[i].first != rank) {
                    tmp_proc[i].second = tmp_proc[i].second + tmp_proc[i].second;
                } else {
                    tmp_proc[i].second = count;
                }
            }
            process_num_map = tmp_proc;
            // cout << "Process " << rank << " have process_num_map: "<< flush << endl;
            // copy(process_num_map.begin(), process_num_map.end(), ostream_iterator<double>(cout, " "));
            // cout << flush << endl;
        }
    }
    if (rank == process_num_map[0].first) {
        // cout << "Process " << rank << " have result vector size: "<< count << flush << endl;
        // copy(result.begin(), result.end(), ostream_iterator<double>(cout, " "));
        // cout << flush << endl;

        MPI_Send(&result[0], count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        MPI_Status status;
        vector<double> tmp(static_cast<int>(vect.size()));

        // cout << "Process " << rank << " await for result: "<< flush << endl;

        MPI_Recv(&tmp[0], static_cast<int>(vect.size()),
            MPI_DOUBLE, process_num_map[0].first, 0, MPI_COMM_WORLD, &status);
        return tmp;
    }
    return vector<double>();
}

std::vector<double> merge(std::vector<double> vect_a, std::vector<double> vect_b) {
    int size1 = static_cast<int>(vect_a.size());
    int size2 = static_cast<int>(vect_b.size());
    std::vector<double> res((size1 + size2));

    int i, j, k;
    i = j = k = 0;
    while (i < size1 && j < size2) {
        if (vect_a[i] < vect_b[j])
            res[k] = vect_a[i++];
        else
            res[k] = vect_b[j++];
        k++;
    }
    while (i < size1) {
        res[k++] = vect_a[i++];
    }
    while (j < size2) {
        res[k++] = vect_b[j++];
    }

    return res;
}
