#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <range/v3/action/sort.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/filter.hpp>

#include "ip_filter.h"
#include "profile.h"

void test() {
    using namespace ranges;

    size_t cnt = 100'000;
    std::vector<IP> ip_pool(cnt);
    for(size_t i = 0; i < cnt; ++i) {
        ip_pool[i] = {rand() % 255, rand() % 255, rand() % 255, rand() % 255};
    }
    auto ip_pool2 = ip_pool;

    {
        LOG_DURATION("original");
        sort_ip_pool(ip_pool.rbegin(), ip_pool.rend());
        std::ostringstream os;
        print_ip_pool(ip_pool, os);
        os << '\n';
    }
    {
        LOG_DURATION("range");
        ip_pool2 |= actions::sort([](const IP& lhs, const IP& rhs) {
        return std::make_tuple(lhs.v1, lhs.v2, lhs.v3, lhs.v4) > std::make_tuple(rhs.v1, rhs.v2, rhs.v3, rhs.v4);
        });
        std::ostringstream os;
        os << views::all(ip_pool) << '\n';
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    test();
    return 0;

    try {
        std::ifstream in("ip_filter.tsv");
        if(!in)
            throw std::runtime_error("file not found");

        std::vector<IP> ip_pool = read_ip_pool(in);

        using namespace ranges;

        // TODO reverse lexicographically sort
        //sort_ip_pool(ip_pool.rbegin(), ip_pool.rend());
        ip_pool |= actions::sort([](const IP& lhs, const IP& rhs) {
        return std::make_tuple(lhs.v1, lhs.v2, lhs.v3, lhs.v4) > std::make_tuple(rhs.v1, rhs.v2, rhs.v3, rhs.v4);
        });

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8
        std::cout << views::all(ip_pool) << '\n';
        std::cout << std::endl;
        std::cout << std::endl;

        // TODO filter by first byte and output
        // ip = filter(1)

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8
        auto pool = ip_pool | views::filter([](const IP &ip) { return ip.v1 == 1; });
        std::cout << pool << '\n';
        std::cout << std::endl;
        std::cout << std::endl;

        // TODO filter by first and second bytes and output
        // ip = filter(46, 70)

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        auto pool2 = ip_pool | views::filter([](const IP &ip) { return ip.v1 == 46 && ip.v2 == 70; });
        std::cout << pool2 << '\n';
        std::cout << std::endl;
        std::cout << std::endl;

        // TODO filter by any byte and output
        // ip = filter_any(46)

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
        auto pool3 = ip_pool | views::filter([](const IP &ip) { return ip.v1 == 46 || ip.v2 == 46 || ip.v3 == 46 || ip.v4 == 46; });
        std::cout << pool3 << '\n';
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}