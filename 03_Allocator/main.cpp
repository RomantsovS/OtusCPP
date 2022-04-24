#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "MyAllocator.h"
#include "vector.h"

#define UNUSED(expr)  \
    do {              \
        (void)(expr); \
    } while (0)

int fact(int n);

int main() {
    {
        using map_type = std::map<int, int, std::less<int>, logging_allocator<std::pair<const int, int>, 12>>;
        map_type m;
        // Visual studio вызывает дополнительую аллокацию, поэтому размер 12.

        for (size_t i = 0; i < 10; ++i) {
            m[i] = fact(i);
        }
        for(auto [key, value] : m) {
            std::cout << key << ' ' << value << std::endl;
        }
    }
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
    {
        auto v = Vector<int, logging_allocator<int, 16>>{};
        for (size_t i = 0; i < 5; ++i) {
            std::cout << "vector size = " << v.size() << std::endl;
            v.push_back(i);
            std::cout << std::endl;
        }
    }
    
    return 0;
}