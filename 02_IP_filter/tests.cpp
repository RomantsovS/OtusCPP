#include <gtest/gtest.h>

#include "ip_filter.h"

TEST(TestAll, read_ip_pool) {
    const std::string str = R"(113.162.145.156	111	0
157.39.22.224	5	6)";

    std::istringstream iss(str);

    auto ip_pool = read_ip_pool(iss);
    EXPECT_EQ(ip_pool.size(), 2u);

    EXPECT_EQ(ip_pool[0].v1, 113);
    EXPECT_EQ(ip_pool[0].v2, 162);
    EXPECT_EQ(ip_pool[0].v3, 145);
    EXPECT_EQ(ip_pool[0].v4, 156);

    EXPECT_EQ(ip_pool[1].v1, 157);
    EXPECT_EQ(ip_pool[1].v2, 39);
    EXPECT_EQ(ip_pool[1].v3, 22);
    EXPECT_EQ(ip_pool[1].v4, 224);
}

TEST(TestAll, print_ip_pool) {
    std::vector<IP> ip_pool = {{1, 2, 3, 4}, {5, 6, 7, 8}};

    std::ostringstream oss;

    print_ip_pool(ip_pool, oss);

    const std::string expected = R"(1.2.3.4
5.6.7.8
)";

    EXPECT_EQ(oss.str(), expected);
}