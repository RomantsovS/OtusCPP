#include <iostream>

#include "async.h"

#include <future>

int main(int, char *[]) {
    auto f1 = std::async(std::launch::async, []() {
        auto h = async::connect(5, std::cout);

        async::receive(h, "cmd1", 4);
        async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
        async::receive(h, "b\nc\nd\n}\n89\n", 11);

        async::disconnect(h);
    });

    auto f2 = std::async(std::launch::async, []() {
        auto h2 = async::connect(5, std::cout);

        async::receive(h2, "cmd6", 4);
        async::receive(h2, "\n7\n8\n9\n", 7);

        async::disconnect(h2);
    });

    f1.get();
    f2.get();

    return 0;
}
