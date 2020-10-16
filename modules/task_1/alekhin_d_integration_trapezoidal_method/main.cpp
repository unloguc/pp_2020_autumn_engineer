// Copyright 2020 Alekhin Denis
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "../../modules/task_1/alekhin_d_integration_trapezoidal_method/integration.h"

TEST(Function_Test, MyFunc_Return_0_From_0) {
  EXPECT_EQ(f(0), 0);
}

TEST(Function_Test, MyFunc_Return_50_From_100) {
  EXPECT_EQ(f(100), 50);
}

TEST(Local_Integration_Test, Local_Integration_Dont_Throws_Exceptions) {
  double func_x[] = {1, 2, 3};
  double func_y[] = {2, 4, 6};


  EXPECT_NO_THROW(localIntegration(func_x, func_y, 3));
}

TEST(Local_Integration_Test, Local_Integration_Return_Right_Value) {
  double func_x[] = {1, 2, 3};
  double func_y[] = {2, 4, 6};

  EXPECT_EQ(localIntegration(func_x, func_y, 3), 8);
}

TEST(Local_Integration_Test, Local_Integration_Return_Right_Value_2) {
  double func_x[] = { 1, 3 };
  double func_y[] = { 2, 6 };

  EXPECT_EQ(localIntegration(func_x, func_y, 2), 8);
}

TEST(Integration, Integration_Dont_Throws_Exceptions) {
  EXPECT_NO_THROW(integration(f, 6, 1, 7));
}

TEST(Integration, Integration_Throws_Exceptions_When_Number_Of_Fragmentation_Less_1) {
  EXPECT_ANY_THROW(integration(f, 0, 1, 3));
}

TEST(Integration, Integration_Throws_Exceptions_When_A_MORE_B) {
  EXPECT_ANY_THROW(integration(f, 4, 4, 3));
}

TEST(Integration, Integration_Return_Right_Value) {
  EXPECT_EQ(integration(f, 6, 1, 7), 12);
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  MPI_Init(&argc, &argv);

  return RUN_ALL_TESTS();
}
