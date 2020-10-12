#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "integration.h"

TEST(Function_Test, MyFunc_Return_0_From_0)
{
	EXPECT_EQ(f(0), 0);
}

TEST(Function_Test, MyFunc_Return_50_From_100)
{
	EXPECT_EQ(f(100), 50);
}

TEST(Sequential_Integration_Test, Sequential_Integration_Return_Half_A_4_From_0_1)
{
	EXPECT_EQ(integrationSequentialMethod(f, 5, 0, 1), 1/4);
}

TEST(Sequential_Integration_Test, Sequential_Integration_Return_25_From_0_10)
{
	EXPECT_EQ(integrationSequentialMethod(f, 5, 0, 10), 25);
}

TEST(Sequential_Integration_Test, Sequential_Integration_Works_Fine_With_Huge_Numbers_Of_Segments)
{
	EXPECT_NO_THROW(integrationSequentialMethod(f, 999999, 0, 1););
	EXPECT_EQ(integrationSequentialMethod(f, 5, 0, 1), 1 / 4);
}

TEST(Sequential_Integration_Test, Sequential_Integration_Works_Fine_With_a_equals_b)
{
	EXPECT_EQ(integrationSequentialMethod(f, 5, 5, 5), 0);
}

TEST(Sequential_Integration_Test, Sequential_Integration_Throws_Error_If_Numbers_Of_Segments_Less_1)
{
	EXPECT_ANY_THROW(integrationSequentialMethod(f, 0, 0, 1););
}

TEST(Sequential_Integration_Test, Sequential_Integration_Throws_Error_If_a_more_than_b)
{
	EXPECT_ANY_THROW(integrationSequentialMethod(f, 5, 5, 1););
}
