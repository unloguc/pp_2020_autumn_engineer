// Copyright 2020 Maksimov Andrey
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>

#include "../../modules/task_3/maksimov_a_mark_components/mark_components.h"

TEST(Parallel_Operations_MPI, Can_Generate_Binary_Image) {
    const int height = 20;
    const int width = 10;

    std::vector<uint32_t> image = getRandomBinaryImage(height, width);
    ASSERT_EQ(static_cast<int>(image.size()), height * width);
    for (int i = 0; i < height * width; i++) {
        ASSERT_LE(image[i], static_cast<uint32_t>(1));
        ASSERT_GE(image[i], static_cast<uint32_t>(0));
    }
}

TEST(Parallel_Operations_MPI, Test_markComponentsNotParall) {
    const int height = 10;
    const int width = 10;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::vector<uint32_t> image = {
            1, 1, 0, 1, 0, 1, 1, 0, 0, 0,
            0, 0, 0, 1, 1, 0, 1, 1, 0, 0,
            0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 0, 1, 1, 1, 0, 0, 0,
            1, 0, 1, 1, 1, 1, 1, 0, 0, 1,
            0, 1, 1, 0, 0, 0, 0, 1, 0, 0,
            0, 0, 0, 1, 1, 0, 1, 1, 1, 0,
            1, 1, 1, 1, 0, 0, 1, 0, 1, 0,
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
        std::vector<uint32_t> predictedImage = {
            1, 1, 0, 2, 0, 3, 3, 0, 0, 0,
            0, 0, 0, 2, 2, 0, 3, 3, 0, 0,
            0, 0, 2, 2, 2, 2, 0, 0, 0, 0,
            4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            4, 4, 4, 0, 4, 4, 4, 0, 0, 0,
            4, 0, 4, 4, 4, 4, 4, 0, 0, 5,
            0, 4, 4, 0, 0, 0, 0, 6, 0, 0,
            0, 0, 0, 7, 7, 0, 6, 6, 6, 0,
            7, 7, 7, 7, 0, 0, 6, 0, 6, 0,
            7, 0, 7, 0, 8, 0, 6, 0, 6, 0 };
        std::vector<uint32_t> resultImage = markComponentsNotParall(image, height, width);
        ASSERT_EQ(predictedImage, resultImage);
    }
}

TEST(Parallel_Operations_MPI, Test_Size_1x1) {
    const int height = 1;
    const int width = 1;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<uint32_t> image;
    if (rank == 0) {
        image = getRandomBinaryImage(height, width);
    }
    std::vector<uint32_t> resultImage = markComponents(image, height, width);
    if (rank == 0) {
        std::vector<uint32_t> resultImageNotParall = markComponentsNotParall(image, height, width);
        ASSERT_EQ(resultImageNotParall, resultImage);
    }
}

TEST(Parallel_Operations_MPI, Test_Size_1x2) {
    const int height = 1;
    const int width = 2;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<uint32_t> image;
    if (rank == 0) {
        image = getRandomBinaryImage(height, width);
    }
    std::vector<uint32_t> resultImage = markComponents(image, height, width);
    if (rank == 0) {
        std::vector<uint32_t> resultImageNotParall = markComponentsNotParall(image, height, width);
        ASSERT_EQ(resultImageNotParall, resultImage);
    }
}

TEST(Parallel_Operations_MPI, Test_Size_5x5) {
    const int height = 5;
    const int width = 5;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<uint32_t> image;
    if (rank == 0) {
        image = getRandomBinaryImage(height, width);
    }
    std::vector<uint32_t> resultImage = markComponents(image, height, width);
    if (rank == 0) {
        std::vector<uint32_t> resultImageNotParall = markComponentsNotParall(image, height, width);
        ASSERT_EQ(resultImageNotParall, resultImage);
    }
}

TEST(Parallel_Operations_MPI, Test_Size_13x11) {
    const int height = 13;
    const int width = 11;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<uint32_t> image;
    if (rank == 0) {
        image = getRandomBinaryImage(height, width);
    }
    std::vector<uint32_t> resultImage = markComponents(image, height, width);
    if (rank == 0) {
        std::vector<uint32_t> resultImageNotParall = markComponentsNotParall(image, height, width);
        ASSERT_EQ(resultImageNotParall, resultImage);
    }
}

TEST(Parallel_Operations_MPI, Test_Size_41x17) {
    const int height = 41;
    const int width = 17;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<uint32_t> image;
    if (rank == 0) {
        image = getRandomBinaryImage(height, width);
    }
    std::vector<uint32_t> resultImage = markComponents(image, height, width);
    if (rank == 0) {
        std::vector<uint32_t> resultImageNotParall = markComponentsNotParall(image, height, width);
        ASSERT_EQ(resultImageNotParall, resultImage);
    }
}

TEST(Parallel_Operations_MPI, Test_Size_53x25) {
    const int height = 53;
    const int width = 25;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<uint32_t> image;
    if (rank == 0) {
        image = getRandomBinaryImage(height, width);
    }
    std::vector<uint32_t> resultImage = markComponents(image, height, width);
    if (rank == 0) {
        std::vector<uint32_t> resultImageNotParall = markComponentsNotParall(image, height, width);
        ASSERT_EQ(resultImageNotParall, resultImage);
    }
}

int main(int argc, char* argv[]) {
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
