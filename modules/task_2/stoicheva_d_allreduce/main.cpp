// Copyright 2020 Stoicheva Darya
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <random>
#include <ctime>
#include "allreduce_mpi.h"


void print_vector(std::vector<int> vector, const size_t size, std::string prefix) {
#ifdef DEBUG_PRINT
    std::cout << "\n" << prefix << "Vector: {\n";
    std::cout << prefix << "  ";
    for (size_t i = 0; i < size; i++) {
        std::cout << vector[i] << ", ";
    }
    std::cout << "\n" << prefix << "}\n";
#endif
}


MPI_Comm create_topology(int root_id) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int root_neighbors_count = ProcNum - 1;
    std::vector<int> index(ProcNum);
    std::vector<int> edges(2 * root_neighbors_count);
    index[0] = root_neighbors_count;
    int edge = 0;
    for (int node = 1; node < ProcNum; node++) {
        index[node] = index[node - 1] + 1;
    }
    for (int proc = 0; proc < ProcNum; proc++) {
        if (proc != root_id)
            edges[edge++] = proc;
    }
    for (int proc = 0; proc < ProcNum - 1; proc++) {
            edges[edge++] = root_id;
    }

#ifdef DEBUG_PRINT
    printf("\nIndex:");
    print_vector(index, index.size(), "[" + std::to_string(ProcRank) + "] ");
    printf("\nEdges:");
    print_vector(edges, edges.size(), "[" + std::to_string(ProcRank) + "] ");
#endif

    MPI_Comm StarComm;
    MPI_Graph_create(MPI_COMM_WORLD, ProcNum, index.data(), edges.data(), 1, &StarComm);

    return StarComm;
}


std::vector<int> create_random_vector(size_t size) {
    assert(size >= 0);
    std::vector<int> vector(size);
    std::mt19937 gen(uint32_t(time(0)));
    std::uniform_int_distribution<int> uid(-50, +50);

    for (size_t i = 0; i < size; i++) {
        vector[i] = uid(gen);
    }

    return vector;
}


void test_with(const size_t size) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int count_elements_per_process = std::max(1, int(size / ProcNum));
    int count_tail_elements = size - count_elements_per_process * ProcNum;
    MPI_Status status;

#ifdef DEBUG_PRINT
    printf("[%d] Process: %d of %d, Vector size: %d, Elements per proc: %d\n", ProcRank, ProcRank, ProcNum, size, count_elements_per_process);
#endif


    int root_id = 0;
    std::vector<int> vector;
    std::vector<int> subvector_recv;
    if (ProcRank == root_id) {
        vector = create_random_vector(size);
        print_vector(vector, size, "[" + std::to_string(ProcRank) + "]");

        std::vector<int> subvector_send;
        for (int proc = 0; proc < ProcNum; proc++) {
            subvector_send = std::vector<int>(vector.begin() + proc * count_elements_per_process,
                vector.begin() + (proc + 1) * count_elements_per_process);
            print_vector(subvector_send, subvector_send.size(), "[" + std::to_string(ProcRank) + "] subvector_send for proc[" +
                std::to_string(proc) + "]");
            if (proc != root_id) {

                MPI_Send(subvector_send.data(), subvector_send.size(), MPI_INT, proc, 0, MPI_COMM_WORLD);

            }
            else {
                subvector_recv.insert(subvector_recv.end(), subvector_send.begin(), subvector_send.end());
            }
        }

        //subvector_recv = std::vector<int>(vector.begin() + ProcNum * count_elements_per_process, vector.end);
        std::vector<int> vector_tail(vector.begin() + ProcNum * count_elements_per_process, vector.end());
        print_vector(vector_tail, vector_tail.size(), "[" + std::to_string(ProcRank) + "] vector_tail ");


        subvector_recv.insert(subvector_recv.end(), vector_tail.begin(), vector_tail.end());
        print_vector(subvector_recv, subvector_recv.size(), "[" + std::to_string(ProcRank) + "] subvector_recv ");

    }
    else {
        subvector_recv.resize(count_elements_per_process);
        MPI_Recv(subvector_recv.data(), count_elements_per_process, MPI_INT, root_id, 0, MPI_COMM_WORLD, &status);
        print_vector(subvector_recv, subvector_recv.size(), "[" + std::to_string(ProcRank) + "] subvector_recv ");
    }
    std::vector<int> vector_out(count_elements_per_process);


    MPI_Comm topology = create_topology(root_id);
    MPI_Op operation = MPI_SUM;

    MPI_Reduce(subvector_recv.data(), vector_out.data(), count_elements_per_process, MPI_INT, operation, root_id, MPI_COMM_WORLD);
    //MPI_Reduce(subvector_recv.data(), vector_out.data(), count_elements_per_process, MPI_INT, operation, root_id, MPI_COMM_WORLD);

    if (ProcRank == root_id) {
        print_vector(vector_out, vector_out.size(), "[" + std::to_string(ProcRank) + "] vector_out ");

        int sum = 0;
        for (int i = 0; i < vector.size(); i++) {
            sum += vector[i];
        }
        printf("Sum of elements: %d\n", sum);
    }
    //int parallel_sum;
    //for (int proc = 0; proc < ProcNum; proc++) {
    //    if (proc != root_id) {
    //        //MPI_Reduce(vector.data() + proc * count_elements_per_process, vector_out.data(), count_elements_per_process, MPI_INT, operation, root_id, topology);
    //        std::vector<int> subvector(vector.begin() + proc * count_elements_per_process, vector.begin() + (proc + 1) * count_elements_per_process);
    //        print_vector(subvector, vector_out.size(), "[" + std::to_string(ProcRank) + "] subvector ");
    //        MPI_Reduce(subvector.data(), vector_out.data(), count_elements_per_process, MPI_INT, operation, root_id, MPI_COMM_WORLD);
    //        //MPI_Reduce(vector.data() + proc * count_elements_per_process, vector_out.data(), count_elements_per_process, MPI_INT, operation, root_id, MPI_COMM_WORLD);
    //        print_vector(vector_out, vector_out.size(), "[" + std::to_string(ProcRank) + "] out ");
    //    }
    //}
    //
    ////    get_max_elements_of_rows_parallel(vector, rows, columns);

    //if (ProcRank == root_id) {
    //    int sequential_sum = 
    //        getSequentialOperations(vector, "+");
    //    ASSERT_EQ(sequential_sum, parallel_sum);
    //}
}


//TEST(Parallel_Operations_MPI, Test_Empty) {
//    test_with(0);
//}
//
//TEST(Parallel_Operations_MPI, Test_One_Element) {
//    test_with(1);
//}

TEST(Parallel_Operations_MPI, Test_Five_Elements) {
    test_with(50);
}

//TEST(Parallel_Operations_MPI, Test_One_Row_Ten_Columns) {
//    test_with(1, 10);
//}
//
//TEST(Parallel_Operations_MPI, Test_Two_Rows_One_Column) {
//    test_with(2, 1);
//}
//
//TEST(Parallel_Operations_MPI, Test_Ten_Rows_One_Column) {
//    test_with(10, 1);
//}
//
//TEST(Parallel_Operations_MPI, Test_Ten_Rows_Ten_Column) {
//    test_with(10, 10);
//}
//
//TEST(Parallel_Operations_MPI, Test_100_Rows_200_Columns) {
//  test_with(100, 200);
//}
//
//TEST(Parallel_Operations_MPI, Test_1000_Rows_2000_Columns) {
//  test_with(1000, 2000);
//}


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
