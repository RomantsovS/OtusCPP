#include "ip_filter.h"

#include <iostream>
#include <sstream>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string& str, char d) {
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos) {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

std::vector<IP> read_ip_pool(std::istream& in) {
    std::vector<IP> res;
    res.reserve(100);

    for (std::string line; std::getline(in, line);) {
        std::vector<std::string> v = split(line, '\t');

        std::istringstream oss(v.at(0));

        IP ip;
        oss >> ip.v1;
        oss.ignore(1);
        oss >> ip.v2;
        oss.ignore(1);
        oss >> ip.v3;
        oss.ignore(1);
        oss >> ip.v4;
        oss.ignore(1);

        res.push_back(std::move(ip));
    }

    return res;
}

std::ostream& operator<<(std::ostream& out, const IP& ip) {
    out << ip.v1 << '.' << ip.v2 << '.' << ip.v3 << '.' << ip.v4;
    return out;
}

void print_ip_pool(const std::vector<IP>& ip_pool, std::ostream& out) {
    for (const auto& ip : ip_pool) {
        out << ip << '\n';
    }
}