// Copyright 2020 Egorov Kirill
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "./circle_comm.h"

TEST(neigbors_tests, correct_amount_of_neigbors) {
    int rank, numtasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Comm comm1 = circle_comm_create();

    if (rank == 0) {
        int nNum;

        for (int i = 0; i < numtasks; i++) {
            MPI_Graph_neighbors_count(comm1, i, &nNum);
            ASSERT_EQ(nNum, 2);
        }
    }
}

TEST(neigbors_tests, neigbors_order) {
    int rank, numtasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Comm comm1 = circle_comm_create();

    if (rank == 0) {
        int nList[2], lEdg = 0, rEdg = 0;

        for (int i = 0; i < numtasks; i++) {
             MPI_Graph_neighbors(comm1, i, 2, &nList[0]);

            if ((i - 1) >= 0 && (i + 1) < numtasks) {
                lEdg = i - 1;
                rEdg = i + 1;
            } else {
                if (i - 1 < 0) {
                    lEdg = numtasks - 1;
                    rEdg = i + 1;
                }
                if (i + 1 >= numtasks) {
                    lEdg = i - 1;
                    rEdg = 0;
                }
            }
            ASSERT_EQ(nList[0], lEdg);
            ASSERT_EQ(nList[1], rEdg);
        }
    }
}

TEST(communications_tests, message_rank_send_test) {
    int rank, numtasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Comm comm1 = circle_comm_create();
    MPI_Status status;

    int nRank;
    int nList[2];
    int control = 0;

    MPI_Comm_rank(comm1, &nRank);

    MPI_Graph_neighbors(comm1, nRank, 2, &nList[0]);
    MPI_Send(&nRank, 1, MPI_INT, nList[1], 0, comm1);
    MPI_Recv(&control, 1, MPI_INT, nList[0], 0, comm1, &status);

    ASSERT_EQ(control, nList[0]);
}

TEST(communications_tests, message_sum_test) {
    int rank, numtasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Comm comm1 = circle_comm_create();
    MPI_Status status;

    int nRank;
    int nList[2];
    int control = 0;

    MPI_Comm_rank(comm1, &nRank);

    MPI_Graph_neighbors(comm1, nRank, 2, &nList[0]);
    if (nRank == 0) {
        MPI_Send(&control, 1, MPI_INT, nList[1], 0, comm1);
    }
    MPI_Recv(&control, 1, MPI_INT, nList[0], 0, comm1, &status);
    control++;
    MPI_Send(&control, 1, MPI_INT, nList[1], 0, comm1);

    if (nRank == 0) {
        ASSERT_EQ(control, numtasks);
    }
}

TEST(communications_tests, message_pass_test) {
    int rank, numtasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Comm comm1 = circle_comm_create();
    MPI_Status status;

    int nRank;
    int nList[2];
    char control = 'a';

    MPI_Comm_rank(comm1, &nRank);

    MPI_Graph_neighbors(comm1, nRank, 2, &nList[0]);
    if (nRank == 0) {
        MPI_Send(&control, 1, MPI_UNSIGNED_CHAR, nList[1], 0, comm1);
    }
    MPI_Recv(&control, 1, MPI_UNSIGNED_CHAR, nList[0], 0, comm1, &status);
    MPI_Send(&control, 1, MPI_UNSIGNED_CHAR, nList[1], 0, comm1);

    if (nRank == 0) {
        ASSERT_EQ(control, 'a');
    }
}

TEST(communications_tests, message_pass_reverse_test) {
    int rank, numtasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Comm comm1 = circle_comm_create();
    MPI_Status status;

    int nRank;
    int nList[2];
    char control = 'a';

    MPI_Comm_rank(comm1, &nRank);

    MPI_Graph_neighbors(comm1, nRank, 2, &nList[0]);
    if (nRank == 0) {
        MPI_Send(&control, 1, MPI_UNSIGNED_CHAR, nList[0], 0, comm1);
    }
    MPI_Recv(&control, 1, MPI_UNSIGNED_CHAR, nList[1], 0, comm1, &status);
    MPI_Send(&control, 1, MPI_UNSIGNED_CHAR, nList[0], 0, comm1);

    if (nRank == 0) {
        ASSERT_EQ(control, 'a');
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
