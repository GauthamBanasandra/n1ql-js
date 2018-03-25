//
// Created by gauth on 24-03-2018.
//

#include <gtest/gtest.h>

TEST(AdditionTest, Addition) {
  EXPECT_EQ(0, 0);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}