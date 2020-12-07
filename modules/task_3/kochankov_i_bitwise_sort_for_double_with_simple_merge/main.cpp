// Copyright 2020 Kochankov Ilya

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./bitwise_sort_for_double_with_simple_merge.h"


using namespace std;

TEST(Parallel_Operations_MPI, getRandomVector_negative_size) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::vector<double> vec;
  const int size = -999;
  if (rank == 0) {
    ASSERT_ANY_THROW(getRandomVector(size));
  }
}

TEST(Parallel_Operations_MPI, getRandomVector_works) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::vector<double> vec;
  const int size = 100;
  if (rank == 0) {
    vec = getRandomVector(size);
    ASSERT_EQ(vec.size(), 100);
  }
}

TEST(Parallel_Operations_MPI, get_digit_works) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double number = 12.2;
  if (rank == 0) {
    int digit_1 = get_digit(number, -1);
    int digit_2 = get_digit(number, 0);
    int digit_3 = get_digit(number, 1);

    EXPECT_EQ(digit_1, 2);
    EXPECT_EQ(digit_2, 2);
    EXPECT_EQ(digit_3, 1);

    int digit_4 = get_digit(91234.34234, -5);
    EXPECT_EQ(digit_4, 4);
  }
}

TEST(Parallel_Operations_MPI, bitwise_sort_works) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    vector<double> vect = {1, 6, 3, 9, 2};
    vector<double> result = {1, 2, 3, 6, 9};
    auto sorted = bitwise_sort(vect, 0);
    EXPECT_EQ(sorted, result);
  }
}

TEST(Parallel_Operations_MPI, bitwise_sort_works_double) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    vector<double> vect = {0.1, 0.6, 0.3, 0.9, 0.2};
    vector<double> result = {0.1, 0.2, 0.3, 0.6, 0.9};
    auto sorted = bitwise_sort(vect, -1);
    EXPECT_EQ(sorted, result);
  }
}

TEST(Parallel_Operations_MPI, bitwise_sort_works_double_wrong_digit) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    vector<double> vect = {0.1, 0.6, 0.3, 0.9, 0.2};
    vector<double> result = {0.1, 0.6, 0.3, 0.9, 0.2};
    auto sorted = bitwise_sort(vect, -2);
    EXPECT_EQ(sorted, result);
  }
}

TEST(Parallel_Operations_MPI, get_digit_number_above_zero_works) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    double number = 134234;
    auto koll_digit = get_digit_number_above_zero(number);
    EXPECT_EQ(koll_digit, 6);
  }
}

TEST(Parallel_Operations_MPI, get_digit_number_below_zero_works) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    double number = 91234.342300;
    auto koll_digit = get_digit_number_below_zero(number);
    EXPECT_EQ(koll_digit, 6);
  }
}

TEST(Parallel_Operations_MPI, linear_bitwise_sort_works) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    vector<double> vect = {111.123, 26.333, 3.444, 91234.34234, 32.1, 32.2};
    vector<double> result = {3.444, 26.333, 32.1, 32.2, 111.123, 91234.34234};
    auto sorted = linear_bitwise_sort(vect);
    EXPECT_EQ(sorted, result);
  }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);
    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
