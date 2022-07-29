#include <boost/asio.hpp>
#include <iostream>


namespace ba = boost::asio;

void test_sync_resolver() {
    std::cout << "\ntest_sync_resolver\n" << std::endl;

    ba::io_context io_context;
    ba::ip::tcp::resolver resolver{io_context};
    auto query = ba::ip::tcp::resolver::query{"google.com", "http"};

    try {
        ba::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        ba::ip::tcp::resolver::iterator end;

        if (iter == end) {
            std::cout << "No endpoints were found!" << std::endl;
        }
        else {
            ba::ip::tcp::endpoint ep = *iter;
            std::cout << ep.address().to_string() << ":" << ep.port() << std::endl;
        }
    }
    catch (const boost::system::system_error& err) {
        std::cout << "Exception! " << err.what() << std::endl;
    }

    std::cout << std::endl;
}


void test_async_resolver() {
    std::cout << "\ntest_async_resolver\n" << std::endl;

    ba::io_context io_context;
    ba::ip::tcp::resolver resolver{io_context};
    auto query = ba::ip::tcp::resolver::query{"google.com", "80"};

    std::cout << "before async_resolve" << std::endl;

    resolver.async_resolve(
        query,
        [](const boost::system::error_code& ec, ba::ip::tcp::resolver::iterator iter) {
            if (ec) {
                std::cout << "Error! " << ec.message() << std::endl;
            }
            else {
                std::cout << "Endpoints:" << std::endl;
                for (iter; iter != ba::ip::tcp::resolver::iterator{}; ++iter) {
                    ba::ip::tcp::endpoint ep = *iter;
                    std::cout << ep.address().to_string() << ":" << ep.port() << std::endl;
                }
            }
        }
    );

    io_context.run();

    std::cout << std::endl;
}


int main(int, char *[]) {

    test_sync_resolver();

    test_async_resolver();

    return 0;
}
