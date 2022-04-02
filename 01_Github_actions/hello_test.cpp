#include <gtest/gtest.h>

#include "lib.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect equality.
  EXPECT_GT(version(), 0);
}