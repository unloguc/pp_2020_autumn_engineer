// Copyright 2020 Gavrilov Nikita
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <stdio.h>\
#include <gtest/gtest.h>
#include <gtest-mpi-listener.hpp>
#include <algorithm>
#include "reduce.h"

TEST(Parallel_Operations_MPI, No_Throw) {
    
    ASSERT_NO_THROW(test<int>(MPI_SUM, 0));
}

TEST(Parallel_Operations_MPI, Any_Throw) {

    ASSERT_ANY_THROW(test<int>(MPI_REPLACE, 0));
    ASSERT_ANY_THROW(test<std::string>(MPI_REPLACE, 0));
}

TEST(Parallel_Operations_MPI, Int_Sum) {

    test<int>(MPI_SUM, 0);
}

TEST(Parallel_Operations_MPI, Float_Sum) {

    test<float>(MPI_SUM, 0);
}

TEST(Parallel_Operations_MPI, Double_Prod) {

    test<double>(MPI_PROD, 1);
}

template <typename T>
void test(MPI_Op op, int root) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int size = 1000000;
    
    std::vector<T> inp = getRandomVector<T>(size);

    std::vector<T> res1(size);
    std::vector<T> res2(size);
    MPI_Datatype datatype = getMPIDataType<T>();

    double t1 = MPI_Wtime();

    reduce(&inp[0], &res1[0], size, datatype, op, root, MPI_COMM_WORLD);

    if (rank == root) {
        t1 = MPI_Wtime() - t1;
    }

    double t2 = MPI_Wtime();

    MPI_Reduce(&inp[0], &res1[0], size, datatype, op, root, MPI_COMM_WORLD);

    if (rank == root) {
        t2 = MPI_Wtime() - t2;

        printf("My time = %fl\n", t1);
        printf("MPI time = %fl\n", t2);
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
