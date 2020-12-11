// Copyright 2020 Pasukhin Dmitry

#include <mpi.h>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <cctype>
#include "../../../modules/task_1/pasukhin_d_word_count/word_count.h"

std::string getRandomString(int size) {
  std::mt19937 gen;
  gen.seed(static_cast<unsigned char>(time(0)));
  std::string a_output_string;
  for (int i = 0; i < size; i++) { a_output_string.push_back(gen() % 254 + 1); }
  return a_output_string;
}

int getSequentialOperations(const std::string input_string, const int size, int *is_first_letter, int *is_last_letter) {
  int a_word_sum = 0;

  if (input_string.empty()) {
    return a_word_sum;
  }

  *is_first_letter = std::isgraph(static_cast<unsigned char>(input_string.at(0)));
  unsigned char prev_char = ' ';
  for (int i = 0; i< size; ++i) {
    if (std::isgraph(static_cast<unsigned char>(input_string[i])) &&
       !std::isgraph(prev_char)) {
      a_word_sum++;
    }
    prev_char = input_string[i];
  }
  *is_last_letter = std::isgraph(static_cast<unsigned char>(prev_char));

  return a_word_sum;
}

int getParallelOperations(const std::string global_str, const int size_string) {
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  const int delta = (size_string / size);
  bool is_paralled = size_string >= size;

  if (rank == 0) {
    int a_size_main = size_string / size + size_string % size;
    for (int proc = 1; proc < size && is_paralled; proc++) {
      MPI_Send(&global_str[0] + a_size_main + (static_cast<int64_t>(proc) - 1) * delta, delta,
        MPI_CHAR, proc, 0, MPI_COMM_WORLD);
    }

    int a_vector_size = static_cast<int64_t>(size) * 3;
    std::vector<int> a_side_letter(a_vector_size, 0);
    a_side_letter[0] = (getSequentialOperations(global_str, a_size_main, &a_side_letter[1], &a_side_letter[2]));
    int global_sum = a_side_letter[0];

    if (!is_paralled) {
      return global_sum;
    }

    MPI_Status status;
    for (int proc = 1; proc < size; proc++) {
      MPI_Recv(&a_side_letter[0] + (static_cast<int64_t>(proc)) * 3, 3,
        MPI_INT, proc, 0, MPI_COMM_WORLD, &status);
    }

    for (int i = 3; i < a_vector_size; i += 3) {
      global_sum += a_side_letter[i];
      if (a_side_letter[i - 1] != 0 && a_side_letter[i + 1] != 0)
        global_sum--;
    }

    return global_sum;
  } else if (is_paralled) {
    std::string local_str(delta, 0x0);
    MPI_Status status;
    MPI_Recv(&local_str[0], delta, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
    int first_letter = 0;
    int last_letter = 0;
    int local_sum = getSequentialOperations(local_str, delta, &first_letter, &last_letter);
    std::vector<int> output_vector {local_sum, first_letter, last_letter};
    MPI_Send(&output_vector[0], 3,
      MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  return 0;
}
