#include <gtest/gtest.h>

#include <sstream>
#include <thread>

#include "client.h"

TEST(TestAll, Test1) {
    std::istringstream is(R"(aaba
aaca
aada
)");

    EXPECT_EQ(calc_min_prefix(is, "./Testing1/", 3, 1), 3);
}

TEST(TestAll, Test2) {
    std::istringstream is(R"(a
a)");

    EXPECT_EQ(calc_min_prefix(is, "./Testing2/", 3, 1), 1);
}

TEST(TestAll, Test3) {
    std::istringstream is(R"(a
b)");

    EXPECT_EQ(calc_min_prefix(is, "./Testing3/", 3, 1), 1);
}

TEST(TestAll, Test4) {
    std::istringstream is(R"(aaba
aaca
abda
)");

    EXPECT_EQ(calc_min_prefix(is, "./Testing4/", 3, 1), 3);
}

TEST(TestAll, Test5) {
    std::istringstream is(R"(aaaa
aaea
aaca
aaba
aafa
aada
aaga
aaha
aaia
aaja
)");

    EXPECT_EQ(calc_min_prefix(is, "./Testing5/", 3, 5), 3);
}