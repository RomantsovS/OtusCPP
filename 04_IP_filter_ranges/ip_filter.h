#include <algorithm>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string& str, char d);

struct IP {
    int v1, v2, v3, v4;
};

std::ostream& operator<<(std::ostream& out, const IP& ip);

std::vector<IP> read_ip_pool(std::istream& in);

void print_ip_pool(const std::vector<IP>& ip_pool, std::ostream& out);

template <typename T>
void sort_ip_pool(T begin, T end) {
    std::sort(begin, end, [](const IP& lhs, const IP& rhs) {
        return std::make_tuple(lhs.v1, lhs.v2, lhs.v3, lhs.v4) < std::make_tuple(rhs.v1, rhs.v2, rhs.v3, rhs.v4);
    });
}

template <typename T>
std::vector<IP> ip_pool_filter(const std::vector<IP>& ip_pool, T pred) {
    std::vector<IP> res;
    res.reserve(100);

    std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(res), pred);
    return res;
}
