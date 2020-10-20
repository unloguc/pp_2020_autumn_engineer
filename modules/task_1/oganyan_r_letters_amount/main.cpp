//
// Created by OgRob on 19.10.2020.
//

#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <string>
#include "./letters_amount.h"

void CustomTest(const int test_size) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::string global_str;
  if (rank==0) {
   // std::cout<<"Creating_randomstring_in_process_0"<<std::endl;
    global_str = getRandomString(test_size);

    std::cout<<global_str<<std::endl;
  }

  auto paral_ans = getParallelOperations(global_str, test_size);
  if (rank==0) {
    auto seq_ans = getSequentialOperations(global_str);
    std::cout<<paral_ans<<std::endl;
    std::cout<<seq_ans<<std::endl;
    ASSERT_EQ(seq_ans, paral_ans);
  }

}
TEST(Parallel_Operations_MPI, Test_tiny) {
  CustomTest(1);
}

TEST(Parallel_Operations_MPI, Test_small) {
  CustomTest(5);
}

TEST(Parallel_Operations_MPI, Test_middle) {
  CustomTest(15);
}

TEST(Parallel_Operations_MPI, Test_large) {
  CustomTest(100);
}

TEST(Parallel_Operations_MPI, Test_huge) {
  CustomTest(1000);
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

  //std::cout<<"Run_tests"<<std::endl;
  return RUN_ALL_TESTS();
}
/*
int main() {
  std::cout<<"kek";
  return 0;
}
 */