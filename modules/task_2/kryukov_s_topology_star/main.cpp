// Copyright 2020 Kryukov Sergey
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include "./topology_star.h"


int topologySize = 10;

TEST(topology_star, realy_topology_star) {
    std::vector<int> nodes(topologySize);
    MPI_Comm commstar = createTopologyStar(nodes);
    int status = MPI_UNDEFINED;
    MPI_Topo_test(commstar, &status);
    ASSERT_EQ(status, MPI_GRAPH);
}

TEST(topology_star, topology_star_get_info) {
    std::vector<int> nodes(topologySize);
    MPI_Comm star = createTopologyStar(nodes);
    int nnodes, nedges;
    MPI_Graphdims_get(star, &nnodes, &nedges);
    EXPECT_EQ(nnodes, topologySize);
    EXPECT_EQ(nedges, (topologySize - 1) * 2);
}

TEST(topology_star, message_exchange) {
    std::vector<int> nodes(topologySize);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm star = createTopologyStar(nodes);
    MPI_Comm_rank(star, &rank);
    int source = 3, dest = 0;
    int sendMessage = 0, recvMess = 15;
    MPI_Status status;
    if (rank == source) {
        starSend(&recvMess, 1, MPI_INT, dest, 0, star);
    } else if (rank == dest) {
        starRecv(&sendMessage, 1, MPI_INT, source, 0, star, &status);
        ASSERT_EQ(sendMessage, recvMess);
    }
}
TEST(topology_star, message_exchange1) {
    std::vector<int> nodes(topologySize);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm star = createTopologyStar(nodes);
    MPI_Comm_rank(star, &rank);
    int source = 0, dest = 3;
    int sendMessage = 0, recvMess = 15;
    MPI_Status status;
    if (rank == source) {
        starSend(&recvMess, 1, MPI_INT, dest, 0, star);
    } else if (rank == dest) {
        starRecv(&sendMessage, 1, MPI_INT, source, 0, star, &status);
        ASSERT_EQ(sendMessage, recvMess);
    }
}

TEST(topology_star, message_exchange2) {
    std::vector<int> nodes(topologySize);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm star = createTopologyStar(nodes);
    MPI_Comm_rank(star, &rank);
    int source = 1, dest = 3;
    int sendMessage = 0, recvMess = 15;
    MPI_Status status;
    if (rank == source) {
        starSend(&recvMess, 1, MPI_INT, dest, 0, star);
    } else if (rank == dest) {
        starRecv(&sendMessage, 1, MPI_INT, source, 0, star, &status);
        ASSERT_EQ(sendMessage, recvMess);
    }
}

TEST(topology_star, random_message_exchange) {
    std::vector<int> nodes(topologySize);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int source, dest;
    std::mt19937 generator;
    generator.seed(static_cast<unsigned int>(time(0)));
    source = generator() % size;
    do {
        dest = generator() % size;
    } while (source == dest);
    MPI_Comm star = createTopologyStar(nodes);
    MPI_Comm_rank(star, &rank);
    int sendMessage = 0, recvMess = 15;
    MPI_Status status;
    if (rank == source) {
        starSend(&recvMess, 1, MPI_INT, dest, 0, star);
    } else if (rank == dest) {
        starRecv(&sendMessage, 1, MPI_INT, source, 0, star, &status);
        ASSERT_EQ(sendMessage, recvMess);
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
