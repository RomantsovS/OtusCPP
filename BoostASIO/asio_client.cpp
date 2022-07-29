#include <boost/asio.hpp>

#include <iostream>

namespace ba = boost::asio;

int main(int, char *[]) {

    std::locale::global(std::locale(""));

    try {
        ba::io_context io_context;

        ba::ip::tcp::endpoint ep(
            ba::ip::address::from_string(
                "127.0.0.1"
            ),
            9999
        );
        ba::ip::tcp::socket sock(io_context);

        sock.connect(ep);

        ba::write(sock, ba::buffer("ping", 4));

        char data[4];
        size_t len = sock.read_some(
            ba::buffer(data)
        );
        std::cout << "receive " << len << "=" << std::string{data, len} << std::endl;
    }
    catch(const boost::system::system_error& ex) {
        std::cout << "boost exception! " << ex.what() << std::endl;
    }
    catch(const std::exception& ex) {
        std::cout << "std::exception! " << ex.what() << std::endl;
    }

    return 0;
}
