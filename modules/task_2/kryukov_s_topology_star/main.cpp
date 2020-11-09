// Copyright 2020 Kryukov Sergey
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include "./topology_star.h"

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
