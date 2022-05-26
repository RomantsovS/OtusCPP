#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

using namespace std;

template <typename T, typename std::enable_if<std::is_integral<T>::value, void>::type* = nullptr>
void print_ip(T val) {
    for (size_t i = 0; i < sizeof(T); ++i) {
        if (i > 0) cout << '.';
        auto shift = (sizeof(T) - i - 1) * 8;
        auto v1 = 0xFFll << shift;
        auto v2 = val & v1;
        auto val2 = v2 >> shift;
        cout << val2;
    }
    cout << endl;
}

template <typename T, typename std::enable_if<std::is_same<T, std::string>::value, void>::type* = nullptr>
void print_ip(T val) {
    cout << val << endl;
}

// template <typename T, typename _ = void>
// struct is_container : std::false_type {};

// template <typename... Ts>
// struct is_container_helper {};

// template <typename T>
// struct is_container<
//     T, typename std::conditional<
//            false,
//            is_container_helper<typename T::value_type, typename T::size_type, typename T::allocator_type,
//                                typename T::iterator, typename T::const_iterator, decltype(std::declval<T>().size()),
//                                decltype(std::declval<T>().begin()), decltype(std::declval<T>().end()),
//                                decltype(std::declval<T>().cbegin()), decltype(std::declval<T>().cend())>,
//            void>::type> : public std::true_type {};

template <typename T>
struct is_container : std::false_type {};

template <typename T>
struct is_container<std::vector<T>> : std::true_type {};

template <typename T>
struct is_container<std::list<T>> : std::true_type {};

template <typename T, typename std::enable_if<is_container<T>::value, void>::type* = nullptr>
void print_ip(T val) {
    copy(begin(val), prev(end(val)), ostream_iterator<typename T::value_type>(cout, "."));
    copy(prev(end(val)), end(val), ostream_iterator<typename T::value_type>(cout));
    cout << endl;
}

template <typename T>
void print_ip(tuple<T, T, T, T> val) {
    std::cout << std::get<0>(val) << "." << std::get<1>(val) << "." << std::get<2>(val) << "." << std::get<3>(val);

    cout << endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    print_ip(int8_t{-1});                            // 255
    print_ip(int16_t{0});                            // 0.0
    print_ip(int32_t{2130706433});                   // 127.0.0.1
    print_ip(int64_t{8875824491850138409});          // 123.45.67.89.101.112.131.41
    print_ip(std::string{"Hello, World !"});         // Hello, World!
    print_ip(std::vector<int>{100, 200, 300, 400});  // 100.200.300.400
    print_ip(std::list<short>{400, 300, 200, 100});  // 400.300.200.100
    print_ip(std::make_tuple(123, 456, 789, 0));     // 123.456.789.0
    // print_ip(make_tuple(4, 3, 2, 1u)); // compile error

    return 0;
}