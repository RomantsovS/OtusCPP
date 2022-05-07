#include <iostream>
#include <vector>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>
#include <range/v3/view/all.hpp>
using std::cout;

struct IP {
    int v1, v2, v3, v4;
};

bool operator<(const IP& lhs, const IP& rhs) {
    return std::make_tuple(lhs.v1, lhs.v2, lhs.v3, lhs.v4) < std::make_tuple(rhs.v1, rhs.v2, rhs.v3, rhs.v4);
}

int main()
{
    std::vector<IP> vi{{9, 4, 5, 2}, {9, 1, 0, 2}};
    using namespace ranges;
    vi |= actions::sort([](const IP& lhs, const IP& rhs) {
        return std::make_tuple(lhs.v1, lhs.v2, lhs.v3, lhs.v4) < std::make_tuple(rhs.v1, rhs.v2, rhs.v3, rhs.v4);
    });
    // prints: [0,1,2,3,4,5,6,7,8,9]
    cout << views::all(vi) << '\n';
}