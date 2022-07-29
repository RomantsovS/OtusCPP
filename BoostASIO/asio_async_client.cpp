#include <boost/asio.hpp>

#include <array>
#include <iostream>
#include <string>
#include <thread>

namespace ba = boost::asio;

class AsyncClient {
public:
    AsyncClient(ba::io_context& io_context)
        : m_tcp_socket{io_context}
        , m_resolver{io_context} {

    }

    // Active object? :)
    void get(const std::string& host, const std::string& service) {

        // 1. Need to resolve the host name
        ba::ip::tcp::resolver::query query{host, service};
        m_resolver.async_resolve(
            query,
            [this](const boost::system::error_code &ec, ba::ip::tcp::resolver::iterator it) {
                std::cout << std::this_thread::get_id() << " run resolveHandler\n";
                this->resolveHandler(ec, it);
            }
        );
        for(size_t i = 0; i < 3; ++i) {
            std::cout << std::this_thread::get_id() << " get i: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
    }

private:
    void resolveHandler(const boost::system::error_code &ec, ba::ip::tcp::resolver::iterator it) {
        std::cout << std::this_thread::get_id() << " resolveHandler\n";
        if (ec) {
            std::cout << "resolveHandler - failed! err = " << ec.message() << std::endl;
            return;
        }

        if (it == ba::ip::tcp::resolver::iterator{}) {
            std::cout << "resolveHandler - no address!" << std::endl;
            return;
        }

        m_tcp_socket.async_connect(
            *it,
            [&](const boost::system::error_code &ec) {
                std::cout << std::this_thread::get_id() << " run connectHandler\n";
                this->connectHandler(ec);
            }
        );
        for(size_t i = 0; i < 3; ++i) {
            std::cout << std::this_thread::get_id() << " resolveHandler i: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
    }

    void connectHandler(const boost::system::error_code &ec) {
        std::cout << std::this_thread::get_id() << " connectHandler\n";
        if (ec) {
            std::cout << "connectHandler - failed! err = " << ec.message() << std::endl;
            return;
        }

        const std::string request = "GET /notExists HTTP/1.1\r\n\r\n";
        ba::write(m_tcp_socket, ba::buffer(request));

        m_tcp_socket.async_read_some(
            ba::buffer(m_buffer),
            [&](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                std::cout << std::this_thread::get_id() << " run readHandler\n";
                this->readHandler(ec, bytes_transferred);
            }
        );
        for(size_t i = 0; i < 3; ++i) {
            std::cout << std::this_thread::get_id() << " connectHandler i: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
    }

    void readHandler(const boost::system::error_code &ec, std::size_t bytes_transferred) {
        std::cout << std::this_thread::get_id() << " readHandler\n";
        if (ec) {
            std::cout << "readHandler - failed! err = " << ec.message() << std::endl;
            return;
        }

        if (!bytes_transferred) {
            std::cout << "readHandler - no more data!" << std::endl;
            return;
        }

        std::cout << "[readHandler] - read " << bytes_transferred << " bytes" << std::endl;

        // std::cout.write(m_buffer.data(), bytes_transferred);
        m_tcp_socket.async_read_some(
            ba::buffer(m_buffer),
            [&](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                std::cout << std::this_thread::get_id() << " run readHandler from readHandler\n";
                readHandler(ec, bytes_transferred);
            }
        );
        for(size_t i = 0; i < 3; ++i) {
            std::cout << std::this_thread::get_id() << " readHandler i: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
    }

    ba::ip::tcp::resolver m_resolver;
    ba::ip::tcp::socket m_tcp_socket;
    std::array<char, 4096> m_buffer;

};

int main() {
    std::locale::global(std::locale(""));

    ba::io_context io_context;

    AsyncClient client{ io_context };

    client.get("google.com", "http");

    io_context.run();

    std::cout << "After the io_context.run() " << std::endl;

    return 0;
}





