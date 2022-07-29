#include <boost/asio.hpp>

#include <iostream>
#include <thread>


namespace ba = boost::asio;

void client_session(ba::ip::tcp::socket sock) {
    std::cout << "new client session\n";
    while (true) {
        try {
            char data[4];
            size_t len = sock.read_some(ba::buffer(data));
            std::cout << "receive " << len << "=" << std::string{data, len} << std::endl;
            ba::write(sock, ba::buffer("pong", 4));
        }
        catch (const std::exception &e) {
            std::cerr << "client_session exception! " << e.what() << std::endl;
            break;
        }
    }
}

int main(int, char *[]) {

    std::locale::global(std::locale(""));

    ba::io_context io_context;
    ba::ip::tcp::endpoint ep(
        ba::ip::tcp::v4(), 
        9999
    );

    ba::ip::tcp::acceptor acc(io_context, ep);

    std::cout << "server started\n";
    while (true) {
        auto sock = ba::ip::tcp::socket(io_context);
        acc.accept(sock);
        std::thread(client_session, std::move(sock)).detach();
    }
    std::cout << "server finished\n";

    return 0;
}
