// Copyright 2020 Ryazanov Dmitry
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <random>
#include "./star.h"

TEST(neigbors_tests, correct_amount_of_neigbors) {
  int rank, numtasks;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  MPI_Comm comm1 = star_comm_create();

  if (rank == 0) {
    int nNum;
    MPI_Graph_neighbors_count(comm1, 0, &nNum);
    for (int i = 1; i < numtasks; i++) {
      MPI_Graph_neighbors_count(comm1, 0, &nNum);
      ASSERT_EQ(nNum, 1);
    }
  }
}

TEST(neigbors_tests, star) {
  int rank;
  int size;
  int send;
  int reciev;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm comm1 = star_comm_create();
  if ((size == 1) || (size == 2)) {
    ASSERT_EQ(1, 1);
  } else {
    std::mt19937 point;
    std::uniform_int_distribution<> flag{2, size - 1};
    send = flag(point);
    reciev = 1;
    int sender = 13;
    int reciever;
    if (rank == send) {
      MPI_Send(&sender, 1, MPI_INT, reciev, 0, comm1);
    }
    if (rank == reciev) {
      MPI_Status status;
      MPI_Recv(&reciever, 1, MPI_INT, send, 0, comm1, &status);
      ASSERT_EQ(sender, reciever);
    }
  }
}

TEST(neigbors_tests, star2) {
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size == 1) {
    ASSERT_EQ(true, true);
    return;
  }
  MPI_Comm comm1 = star_comm_create();
  int status = MPI_UNDEFINED;
  MPI_Topo_test(comm1, &status);
  ASSERT_EQ(status, MPI_GRAPH);
}

TEST(neigbors_tests, get_info) {
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if ((size == 1) || (size == 2)) {
    ASSERT_EQ(true, true);
    return;
  }

  MPI_Comm comm1 = star_comm_create();
  int nindex, nedges;
  MPI_Graphdims_get(comm1, &nindex, &nedges);
  EXPECT_EQ(nindex, size);
  EXPECT_EQ(nedges, (size - 2));
}

TEST(communications_tests, message_send_test) {
  int rank;
  int size;
  int send;
  int reciev;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm comm1 = star_comm_create();
  if (size == 1) {
    ASSERT_EQ(1, 1);
  } else {
    std::mt19937 point;
    std::uniform_int_distribution<> flag{1, size - 1};
    send = 0;
    reciev = flag(point);
    int sender = 5;
    int recieve;
    if (rank == send) {
      MPI_Send(&sender, 1, MPI_INT, reciev, 0, comm1);
    }
    if (rank == reciev) {
      MPI_Status status;
      MPI_Recv(&recieve, 1, MPI_INT, send, 0, comm1, &status);
      ASSERT_EQ(sender, recieve);
    }
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
