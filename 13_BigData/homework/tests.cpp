#include <gtest/gtest.h>

#include <sstream>
#include <thread>

#include "client.h"

TEST(TestAll, Test1) {
    std::istringstream is(R"(abc1@gmail.com
bca2@gmail.com
cba3@gmail.com
fsdf@gmail.com
jkj@gmail.com
fgh@gmail.com
xcv@gmail.com)");

    EXPECT_EQ(calc_min_prefix(is, "./Testing/"), 2);
}

TEST(TestAll, Test2) {
    std::istringstream is(R"(a
a)");

    EXPECT_EQ(calc_min_prefix(is, "./Testing/"), 1);
}