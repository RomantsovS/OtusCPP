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

template <class, typename = void>
struct func_dispatch_tag : std::integral_constant<int, 0> {};

template <class C>
struct func_dispatch_tag<C, std::enable_if_t<std::is_integral<C>::value>> : std::integral_constant<int, 1> {};

template <typename T, typename _ = void>
struct is_container : std::false_type {};

template <typename... Ts>
struct is_container_helper {};

template <typename T>
struct is_container<
    T, std::conditional_t<
           false,
           is_container_helper<typename T::value_type, typename T::size_type, typename T::allocator_type,
                               typename T::iterator, typename T::const_iterator, decltype(std::declval<T>().size()),
                               decltype(std::declval<T>().begin()), decltype(std::declval<T>().end()),
                               decltype(std::declval<T>().cbegin()), decltype(std::declval<T>().cend())>,
           void>> : public std::true_type {};

template <class C>
struct func_dispatch_tag<C, std::enable_if_t<is_container<C>::value, void>> : std::integral_constant<int, 2> {};

template <class C>
struct func_dispatch_tag<C, std::enable_if_t<is_constructible<string_view, C>::value, void>> : std::integral_constant<int, 3> {};

template <typename T>
void printIPImpl(T val, std::integral_constant<int, 1>) {
    for (size_t i = 0; i < sizeof(T); ++i) {
        if (i > 0) cout << '.';
        auto shift = (sizeof(T) - i - 1) * 8;
        auto val2 = (val & (0xFF << shift)) >> shift;
        cout << val2;
    }
    cout << endl;
}

template <typename T>
void printIPImpl(T val, std::integral_constant<int, 2>) {
    copy(begin(val), prev(end(val)), ostream_iterator<typename T::value_type>(cout, "."));
    copy(prev(end(val)), end(val), ostream_iterator<typename T::value_type>(cout));
    cout << endl;
}

template <typename T>
void printIPImpl(T val, std::integral_constant<int, 3>) {
    cout << val << endl;
}

template <class C>
void printIP(C c) {
    printIPImpl(c, func_dispatch_tag<C>{});
}

template <>
void printIP(std::string val) {
    cout << val << endl;
}

template <typename T>
void printIP(tuple<T, T, T, T> val) {
    std::cout << std::get<0>(val) << "." << std::get<1>(val) << "." << std::get<2>(val) << "." << std::get<3>(val);

    cout << endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    printIP(char(-1));
    printIP(short(0));
    printIP(int(2130706433));
    printIP(long(8875824491850138409));
    printIP("127.0.0.1"s);
    printIP("127.0.0.1"); // string_view
    printIP(vector<int>{1, 2, 3, 4});
    printIP(list<int>{1, 2, 3, 4});
    printIP(make_tuple(4, 3, 2, 1));
    // printIP(make_tuple(4, 3, 2)); // compile error
    // printIP(make_tuple(4, 3, 2, 1u)); // compile error

    return 0;
}