//  Copyright 2020 Pasukhin Dmitry

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <cmath>
#include <limits>
#include "./hypercube.h"

TEST(Parallel_Operations_MPI, Test_Forward_Sending) {
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sender = randPath(0, size / 2 - 1);
  int reciver = randPath((size / 2), size - 1);

  int mes_size = 1;
  double mes_value = 5.5;
  MPI_Comm hypercube = createHypercubeComm();


  if (rank == sender) {
    std::vector<double> vec{ mes_value };
    MPI_Request req;
    MPI_Isend(&vec[0], vec.size(), MPI_DOUBLE, reciver, 0, hypercube, &req);
  }
  if (rank == reciver) {
    MPI_Status stat;
    double* res = new double[mes_size];
    MPI_Recv(res, mes_size, MPI_DOUBLE, sender, 0, hypercube, &stat);
    ASSERT_DOUBLE_EQ(mes_value, res[0]);
  }
}

TEST(Parallel_Operations_MPI, Test_Reverse_Many_Data_Sending) {
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sender = randPath((size / 2), size - 1);
  int reciver = randPath(0, size / 2 - 1);

  int mes_size = 4;
  double mes_value[] = { 5.5, 4.4, 7.7, 8.8 };
  MPI_Comm hypercube = createHypercubeComm();


  if (rank == sender) {
    MPI_Request req;
    std::vector<double> vec{ 5.5, 4.4, 7.7, 8.8 };
    MPI_Isend(&vec[0], vec.size(), MPI_DOUBLE, reciver, 0, hypercube, &req);
  }
  if (rank == reciver) {
    MPI_Status stat;
    double* res = new double[mes_size];
    MPI_Recv(res, mes_size, MPI_DOUBLE, sender, 0, hypercube, &stat);
    bool isOK = true;
    for (int i = 0; i < 4 && isOK; ++i) {
      if (fabs(mes_value[i] - res[i]) >= std::numeric_limits<double>::epsilon())
        isOK = false;
    }
    ASSERT_EQ(isOK, true);
  }
}

TEST(Parallel_Operations_MPI, Test_Forward_One_Rand_Data_Sending) {
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sender = randPath(0, size / 2 - 1);
  int reciver = randPath((size / 2), size - 1);

  int mes_size = 1;
  double mes_value = randPath(0, 200);
  MPI_Comm hypercube = createHypercubeComm();


  if (rank == sender) {
    MPI_Request req;
    std::vector<double> vec{ mes_value };
    MPI_Isend(&vec[0], vec.size(), MPI_DOUBLE, reciver, 0, hypercube, &req);
  }
  if (rank == reciver) {
    MPI_Status stat;
    double* res = new double[mes_size];
    MPI_Recv(res, mes_size, MPI_DOUBLE, sender, 0, hypercube, &stat);
    ASSERT_DOUBLE_EQ(mes_value, res[0]);
  }
}

TEST(Parallel_Operations_MPI, Test_Reverse_Sending) {
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sender = randPath((size / 2), size - 1);
  int reciver = randPath(0, size / 2 - 1);

  int mes_size = 1;
  double mes_value = 5.5;
  MPI_Comm hypercube = createHypercubeComm();


  if (rank == sender) {
    MPI_Request req;
    std::vector<double> vec{ mes_value };
    MPI_Isend(&vec[0], vec.size(), MPI_DOUBLE, reciver, 0, hypercube, &req);
  }
  if (rank == reciver) {
    MPI_Status stat;
    double* res = new double[mes_size];
    MPI_Recv(res, mes_size, MPI_DOUBLE, sender, 0, hypercube, &stat);
    ASSERT_DOUBLE_EQ(mes_value, res[0]);
  }
}


TEST(Parallel_Operations_MPI, Test_Forward_Many_Data_Sending) {
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sender = randPath(0, size / 2 - 1);
  int reciver = randPath((size / 2), size - 1);

  int mes_size = 4;
  double mes_value[] = { 5.5, 4.4, 7.7, 8.8 };
  MPI_Comm hypercube = createHypercubeComm();


  if (rank == sender) {
    MPI_Request req;
    std::vector<double> vec{ 5.5, 4.4, 7.7, 8.8 };
    MPI_Isend(&vec[0], vec.size(), MPI_DOUBLE, reciver, 0, hypercube, &req);
  }
  if (rank == reciver) {
    MPI_Status stat;
    double* res = new double[mes_size];
    MPI_Recv(res, mes_size, MPI_DOUBLE, sender, 0, hypercube, &stat);
    bool isOK = true;
    for (int i = 0; i < 4 && isOK; ++i) {
      if (fabs(mes_value[i] - res[i]) >= std::numeric_limits<double>::epsilon())
        isOK = false;
    }
    ASSERT_EQ(isOK, true);
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
