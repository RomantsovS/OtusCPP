#include <gtest/gtest.h>

#include "MyAllocator.h"
#include "vector.h"

int fact(int n);

TEST(TestAll, TestAllocator) {
    {
        logging_allocator<int, 10> allocator;
        allocator.allocate(1);
        EXPECT_EQ(allocator.GetAllocCount(), 1u);
        EXPECT_EQ(allocator.GetTotalCount(), 10u);
        allocator.allocate(1);
        EXPECT_EQ(allocator.GetAllocCount(), 2u);
        EXPECT_EQ(allocator.GetTotalCount(), 10u);

        allocator.allocate(8);
        EXPECT_EQ(allocator.GetAllocCount(), 10u);
        EXPECT_EQ(allocator.GetTotalCount(), 10u);

        EXPECT_THROW(allocator.allocate(1), std::runtime_error);
    }
    {
        logging_allocator<int, 10, true> allocator;
        allocator.allocate(1);
        EXPECT_EQ(allocator.GetAllocCount(), 1u);
        EXPECT_EQ(allocator.GetTotalCount(), 10u);
        allocator.allocate(1);
        EXPECT_EQ(allocator.GetAllocCount(), 2u);
        EXPECT_EQ(allocator.GetTotalCount(), 10u);

        allocator.allocate(8);
        EXPECT_EQ(allocator.GetAllocCount(), 10u);
        EXPECT_EQ(allocator.GetTotalCount(), 10u);

        allocator.allocate(1);
        EXPECT_EQ(allocator.GetAllocCount(), 11u);
        EXPECT_EQ(allocator.GetTotalCount(), 20u);
    }
    {
        using map_type = std::map<int, int, std::less<int>, logging_allocator<std::pair<const int, int>, 10>>;
        map_type m;

        for (size_t i = 0; i < 10; ++i) {
            m[i] = fact(i);
        }
        for(auto [key, value] : m) {
            std::cout << key << ' ' << value << std::endl;
        }
    }
    {
        logging_allocator<int, 16> alloc(true);
        auto v = std::vector<int, logging_allocator<int, 16>>(alloc);
        for (size_t i = 0; i < 4; ++i) {
            std::cout << "vector size = " << v.size() << std::endl;
            v.emplace_back(i);
            std::cout << std::endl;
        }
    }
}

TEST(TestAll, TestVector) {
    {
        auto v = Vector<int, std::allocator<int>>{};
        for (size_t i = 0; i < 10; ++i) {
            std::cout << "vector size = " << v.size() << std::endl;
            v.push_back(i);
            std::cout << std::endl;
        }
        for(auto val : v) {
            std::cout << val << std::endl;
        }
        auto v2 = v;
        for(auto val : v2) {
            std::cout << val << std::endl;
        }
    }
}

TEST(TestAll, TestVectorWithMyAlloc) {
    {
        auto v = Vector<int, logging_allocator<int, 16>>{};
        for (size_t i = 0; i < 5; ++i) {
            std::cout << "vector size = " << v.size() << std::endl;
            v.push_back(i);
            std::cout << std::endl;
        }
    }
}