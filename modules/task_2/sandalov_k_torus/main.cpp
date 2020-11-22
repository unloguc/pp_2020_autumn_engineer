// Copyright 2020 Sandalov Konstantin

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

#include "../../../modules/task_2/sandalov_k_torus/torus.h"



TEST(Topology_Torus, Create_topology_with_correct_number_of_processes) {
    int procRank, procNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);

    int dims[] = {0, 0};
    MPI_Dims_create(procNum, 2, dims);
    MPI_Comm torusTopology;
    bool res = createTorusTopology(&torusTopology, dims[0], dims[1]);
    ASSERT_TRUE(res);
    freeTorusTopology(&torusTopology, res);
}

TEST(Topology_Torus, Create_topology_from_extra_number_of_processes) {
    int procRank, procNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);

    int dims[] = {0, 0};
    if (procNum > 1) --procNum;
    MPI_Dims_create(procNum, 2, dims);

    MPI_Comm torusTopology;
    bool res = createTorusTopology(&torusTopology, dims[0], dims[1]);

    if (procRank < procNum) {
        ASSERT_TRUE(res);
    } else {
        ASSERT_FALSE(res);
    }
    freeTorusTopology(&torusTopology, res);
}

TEST(Topology_Torus, Send_Message_To_Random_Point_For_Single_Process) {
    int procRank, procNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    std::vector<int> message;
    int dims[] = {0, 0};
    MPI_Dims_create(procNum, 2, dims);

    MPI_Comm torusTopology;
    bool res = createTorusTopology(&torusTopology, dims[0], dims[1]);
    if (res) {
        MPI_Comm_rank(torusTopology, &procRank);
        MPI_Comm_size(torusTopology, &procNum);

        int startProc = -1;
        int endProc = -1;
        if (procRank == 0) {
            startProc = procRank;
            endProc = takeRandomPoint(dims[0] * dims[1]);
            message = std::vector<int>{1, 2, 3, 4, 5};
        }

        int sendingResult = sendMessageInTorus(&torusTopology, &message, startProc, endProc);
        if (sendingResult == 1) {
            bool receivedRightMessage = true;
            if (message.size() == 5) {
                for (int j = 0; j < 5; ++j) {
                    if (message[j] != j + 1) receivedRightMessage = false;
                }
            } else {
                receivedRightMessage = false;
            }
            ASSERT_TRUE(receivedRightMessage);
        }
    }
    freeTorusTopology(&torusTopology, res);
}

TEST(Topology_Torus, Send_Message_To_Random_Point_For_Each_Process) {
    int procRank, procNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    std::vector<int> message;
    int dims[] = {0, 0};
    MPI_Dims_create(procNum, 2, dims);

    MPI_Comm torusTopology;
    bool res = createTorusTopology(&torusTopology, dims[0], dims[1]);
    if (res) {
        MPI_Comm_rank(torusTopology, &procRank);
        MPI_Comm_size(torusTopology, &procNum);

        for (int i = 0; i < procNum; ++i) {
            int startProc = -1;
            int endProc = -1;
            if (procRank == i) {
                startProc = procRank;
                endProc = takeRandomPoint(dims[0] * dims[1]);
                message = std::vector<int>{1 + i, 2 + i, 3 + i, 4 + i, 5 + i};
            }

            int sendingResult = sendMessageInTorus(&torusTopology, &message, startProc, endProc);
            if (sendingResult == 1) {
                bool receivedRightMessage = true;
                if (message.size() == 5) {
                    for (int j = 0; j < 5; ++j) {
                        if (message[j] != j + i + 1) receivedRightMessage = false;
                    }
                } else {
                    receivedRightMessage = false;
                }
                ASSERT_TRUE(receivedRightMessage);
            }
        }
    }
    freeTorusTopology(&torusTopology, res);
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
