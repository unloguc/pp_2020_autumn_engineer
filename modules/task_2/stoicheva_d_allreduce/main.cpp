// Copyright 2020 Stoicheva Darya
#include <gtest/gtest.h>
#include <gtest-mpi-listener.hpp>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include "../../../modules/task_2/stoicheva_d_allreduce/allreduce_mpi.h"


template<typename T>
std::vector<T> create_random_vector(size_t size) {
    assert(size >= 0);
    std::vector<T> vector(size);
    std::mt19937 gen(uint32_t(time(0)));
    std::uniform_int_distribution<int> uid_int(-50, +50);
    std::uniform_real_distribution<float> uid_real(-50.0, +50.0);

    for (size_t i = 0; i < size; i++) {
        vector[i] = std::is_same<T, int>::value ? uid_int(gen) : uid_real(gen);
    }

    return vector;
}


template<typename T>
void test_allreduce_with(const int count, const int root_id, const MPI_Datatype datatype, const MPI_Op op,
                    bool show_times = false) {
    int world_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm comm = MPI_COMM_WORLD;

#ifdef DEBUG_PRINT
    printf("[%d] Process: %d of %d, Vector size: %d\n", rank, rank, world_size, count);
#endif

    if (root_id < 0 || root_id >= world_size) {
        return;
    }

    std::vector<T> global_data_in(count * world_size);

    if (rank == root_id) {
        global_data_in = create_random_vector<T>(count * world_size);
#ifdef DEBUG_PRINT
        print_vector<T>(global_data_in, global_data_in.size(), "[" + std::to_string(rank) + "]");
#endif
    }

    // Scatter test data between processes
    std::vector<T> proc_data_in(count);
    MPI_Scatter(global_data_in.data(), count, datatype, proc_data_in.data(), count, datatype, root_id, comm);
#ifdef DEBUG_PRINT
    print_vector(proc_data_in, proc_data_in.size(), "[" + std::to_string(rank) + "] proc_data_in ");
#endif

    // Use my function to get result data
    std::vector<T> proc_data_out(count);

    double t1;
    if (rank == root_id) {
        t1 = MPI_Wtime();
    }
    my_MPI_Allreduce(proc_data_in.data(), proc_data_out.data(), count, datatype, op, comm);
    if (rank == root_id) {
        t1 = MPI_Wtime() - t1;
    }
    T proc_out = getSequentialOperations<T>(proc_data_out, op);

    // Use library function to get expected data
    std::vector<T> expected_proc_data_out(count);
    std::vector<T> expected_proc_data_out2(count);

    double t2;
    if (rank == root_id) {
        t2 = MPI_Wtime();
    }
    MPI_Allreduce(proc_data_in.data(), expected_proc_data_out.data(), count, datatype, op, comm);
    if (rank == root_id) {
        t2 = MPI_Wtime() - t2;
    }
    T expected_out = getSequentialOperations<T>(expected_proc_data_out, op);

#ifdef DEBUG_PRINT
    print_vector<T>(expected_proc_data_out, expected_proc_data_out.size(),
        "[" + std::to_string(rank) + "] expected ");
    print_vector<T>(expected_proc_data_out2, expected_proc_data_out2.size(),
        "[" + std::to_string(rank) + "] expected2 ");
    print_vector<T>(proc_data_out, proc_data_out.size(), "[" + std::to_string(rank) + "] test      ");
    printf("[%d] Proc_out = %s   Expected_out = %s\n", rank, std::to_string(proc_out).c_str(),
        std::to_string(expected_out).c_str());
#endif

    if (rank == root_id) {
        if (show_times) {
            std::cout << "my_MPI_Allreduce time: " << t1 << std::endl;
            std::cout << "MPI_Allreduce Time: " << t2 << std::endl;
            std::cout << "MPI_Allreduce " << t1 / t2 << " times faster." << std::endl;
        }

        T sequential_out = getSequentialOperations<T>(global_data_in, op);

#ifdef DEBUG_PRINT
        printf("[%d] sequential_out = %s\n", rank, std::to_string(sequential_out).c_str());
#endif
        if (MPI_INT == datatype) {
            ASSERT_EQ(expected_out, proc_out);
            ASSERT_EQ(sequential_out, proc_out);
        } else if (MPI_FLOAT == datatype) {
            ASSERT_NEAR(expected_out, proc_out, 0.001);
            ASSERT_NEAR(sequential_out, proc_out, 0.001);
        } else if (MPI_DOUBLE == datatype) {
            ASSERT_DOUBLE_EQ(expected_out, proc_out);
            ASSERT_NEAR(sequential_out, proc_out, 0.001);
        }
    }

    ASSERT_EQ(expected_proc_data_out.size(), proc_data_out.size())
        << "Vectors expected_proc_data_out and proc_data_out are of unequal length";

    std::string message("expected_proc_data_out and proc_data_out differ at index ");
    for (size_t i = 0; i < expected_proc_data_out.size(); ++i) {
        if (datatype == MPI_INT) {
            EXPECT_EQ(expected_proc_data_out[i], proc_data_out[i]) << "Vectors<int> " << message << i;
        } else {
            EXPECT_NEAR(expected_proc_data_out[i], proc_data_out[i], 0.001) << "Vectors<float> " << message << i;
        }
    }
}


TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_INT_SUM) {
    test_allreduce_with<int>(1, 0, MPI_INT, MPI_SUM);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R1_INT_SUM) {
    test_allreduce_with<int>(1, 1, MPI_INT, MPI_SUM);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_FLOAT_SUM) {
    test_allreduce_with<float>(1, 0, MPI_FLOAT, MPI_SUM);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_DOUBLE_SUM) {
    test_allreduce_with<double>(1, 0, MPI_DOUBLE, MPI_SUM);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_INT_SUM) {
    test_allreduce_with<int>(6, 0, MPI_INT, MPI_SUM);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_FLOAT_SUM) {
    test_allreduce_with<float>(6, 0, MPI_FLOAT, MPI_SUM);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_DOUBLE_SUM) {
    test_allreduce_with<double>(6, 0, MPI_DOUBLE, MPI_SUM);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_INT_MAX) {
    test_allreduce_with<int>(1, 0, MPI_INT, MPI_MAX);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_FLOAT_MAX) {
    test_allreduce_with<float>(1, 0, MPI_FLOAT, MPI_MAX);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_DOUBLE_MAX) {
    test_allreduce_with<double>(1, 0, MPI_DOUBLE, MPI_MAX);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_INT_MAX) {
    test_allreduce_with<int>(6, 0, MPI_INT, MPI_MAX);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_FLOAT_MAX) {
    test_allreduce_with<float>(6, 0, MPI_FLOAT, MPI_MAX);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_DOUBLE_MAX) {
    test_allreduce_with<double>(6, 0, MPI_DOUBLE, MPI_MAX);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_INT_MIN) {
    test_allreduce_with<int>(1, 0, MPI_INT, MPI_MIN);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_FLOAT_MIN) {
    test_allreduce_with<float>(1, 0, MPI_FLOAT, MPI_MIN);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C1_R0_DOUBLE_MIN) {
    test_allreduce_with<double>(1, 0, MPI_DOUBLE, MPI_MIN);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_INT_MIN) {
    test_allreduce_with<int>(6, 0, MPI_INT, MPI_MIN);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_FLOAT_MIN) {
    test_allreduce_with<float>(6, 0, MPI_FLOAT, MPI_MIN);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C6_R0_DOUBLE_MIN) {
    test_allreduce_with<double>(6, 0, MPI_DOUBLE, MPI_MIN);
}

TEST(DISABLED_Parallel_Operations_MPI, Test_swap_1_0_10) {
    ASSERT_EQ(swap_0_with_root(true, 0, 10), 10);
}

TEST(DISABLED_Parallel_Operations_MPI, Test_swap_1_5_10) {
    ASSERT_EQ(swap_0_with_root(true, 5, 10), 5);
}

TEST(DISABLED_Parallel_Operations_MPI, Test_swap_0_0_10) {
    ASSERT_EQ(swap_0_with_root(false, 0, 10), 0);
}

TEST(DISABLED_Parallel_Operations_MPI, Test_swap_0_5_10) {
    ASSERT_EQ(swap_0_with_root(false, 5, 10), 5);
}

TEST(Parallel_Operations_MPI, Test_Allreduce_C100000_R2_INT_SUM) {
    test_allreduce_with<int>(100000, 0, MPI_INT, MPI_SUM, true);
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
