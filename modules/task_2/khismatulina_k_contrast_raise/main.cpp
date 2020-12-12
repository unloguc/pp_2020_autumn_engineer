//  Copyright 2020 Khismatulina Karina
#include <gtest/gtest.h>
#include <vector>
#include "../../../modules/task_2/khismatulina_k_contrast_raise/contrast_raise.h"
#include "../../../3rdparty/gtest-mpi/linux/gtest-mpi-listener.hpp"

TEST(khism_task_2, test_1) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        ASSERT_ANY_THROW(getRandomImage(-10));
    }
}

TEST(khism_task_2, test_2_seq) {
    int rank;
    int size = 9;
    int contrast = 20;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> res(size);
    std::vector<int> exp(size);
    if (rank == 0) {
        res[0] = 150;
        res[1] = 120;
        res[2] = 100;
        res[3] = 123;
        res[4] = 175;
        res[5] = 170;
        res[6] = 50;
        res[7] = 250;
        res[8] = 70;

        exp[0] = 153;
        exp[1] = 117;
        exp[2] = 93;
        exp[3] = 120;
        exp[4] = 183;
        exp[5] = 177;
        exp[6] = 33;
        exp[7] = 255;
        exp[8] = 57;
    }
    if (rank == 0) {
        int midBright = getMidBright(res, size);
        res = contrastRaiseSeq(res, size, contrast, midBright);
        ASSERT_EQ(exp, res);
    }
}
TEST(khism_task_2, test_3_paral_and_chiselki) {
    int size = 9;
    int contrast = 20;
    std::vector <int> res(size), exp(size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        res[0] = 150;
        res[1] = 120;
        res[2] = 100;
        res[3] = 123;
        res[4] = 175;
        res[5] = 170;
        res[6] = 50;
        res[7] = 250;
        res[8] = 70;

        exp[0] = 153;
        exp[1] = 117;
        exp[2] = 93;
        exp[3] = 120;
        exp[4] = 183;
        exp[5] = 177;
        exp[6] = 33;
        exp[7] = 255;
        exp[8] = 57;
    }
    res = contrastRaiseParallel(res, size, contrast);
    if (rank == 0) {
        ASSERT_EQ(exp, res);
    }
}

TEST(khism_task_2, test_4_paral_versus_seq_small) {
    int rank;
    int size = 4;
    int contrast = 10;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> image = {1, 2, 4, 6};

    std::vector<int> res = contrastRaiseParallel(image, size, contrast);

    if (rank == 0) {
        int midBright = getMidBright(image, size);
        std::vector<int> exp = contrastRaiseSeq(image, size, contrast, midBright);
        ASSERT_EQ(res, exp);
    }
}

TEST(khism_task_2, test_5_paral_versus_seq_big) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size = 1000;
    int contrast = 20;
    std::vector<int> image = getRandomImage(size);

    std::vector<int> res = contrastRaiseParallel(image, size, contrast);

    if (rank == 0) {
        int midBright = getMidBright(image, size);
        std::vector<int> exp = contrastRaiseSeq(image, size, contrast, midBright);
        ASSERT_EQ(res, exp);
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
