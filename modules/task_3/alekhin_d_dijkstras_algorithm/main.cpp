// Copyright 2020 Alekhin Denis
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "../../modules/task_3/alekhin_d_dijkstras_algorithm/dijkstras_algorithm.h"

int graphSize = 500;

TEST(Get_Random_Graph, Get_Random_Graph_Test) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    std::vector<int> graph;
    EXPECT_NO_THROW(getRandomGraph(&graph, graphSize));
  }
}

TEST(Get_Dijkstras_Algorithm_Sequential, Get_Dijkstras_Algorithm_Sequential_Works) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    std::vector<int> graph;
    getRandomGraph(&graph, graphSize);
    std::vector<int> result;
    EXPECT_NO_THROW(result = getDijkstrasAlgorithmSequential(&graph, 0));
  }
}

TEST(Get_Dijkstras_Algorithm_Sequential, Get_Dijkstras_Algorithm_Sequential_Test) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    std::vector<int> graph;
    getRandomGraph(&graph, graphSize);
    std::vector<int> result;

    double startTime = MPI_Wtime();
    result = getDijkstrasAlgorithmSequential(&graph, 0);
    double endTime = MPI_Wtime();
    std::cout << std::endl << endTime - startTime << std::endl;
  }
}

TEST(Get_Dijkstras_Algorithm_Parallel, Get_Dijkstras_Algorithm_Parallel_Works) {
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (size > 1 && size <= graphSize) {
    std::vector<int> graph;
    getRandomGraph(&graph, graphSize);
    std::vector<int> result;
    EXPECT_NO_THROW(result = getDijkstrasAlgorithmParallel(&graph, 0));
    if (rank == 0) {
      // printResult(&result);
    }
  }
}

TEST(Get_Dijkstras_Algorithm_Parallel, Get_Dijkstras_Algorithm_Parallel_Test) {
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (size > 1 && size <= graphSize) {
    std::vector<int> graph;
    getRandomGraph(&graph, graphSize);
    std::vector<int> result;

    double startTime = MPI_Wtime();
    result = getDijkstrasAlgorithmParallel(&graph, 0);
    double endTime = MPI_Wtime();
    if (rank == 0) {
      std::cout << std::endl << endTime - startTime << std::endl;
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
