#pragma once

#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace ba = boost::asio;

class AsyncClient {
   public:
    AsyncClient(ba::io_context &io_context, short port, std::vector<std::string> str)
        : m_resolver{io_context}, m_tcp_socket{io_context}, m_str(std::move(str)) {
        ba::ip::tcp::endpoint ep(ba::ip::address::from_string("127.0.0.1"), port);

        m_tcp_socket.async_connect(ep, [&](const boost::system::error_code &erc) {
            // std::cout << std::this_thread::get_id() << " run connectHandler\n";
            this->connectHandler(erc);
        });
    }

   private:
    void connectHandler(const boost::system::error_code &ec) {
        // std::cout << std::this_thread::get_id() << " connectHandler\n";
        if (ec) {
            std::cout << "connectHandler - failed! err = " << ec.message() << std::endl;
            return;
        }

        for (auto &str : m_str) {
            ba::write(m_tcp_socket, ba::buffer(str));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        boost::system::error_code erc;
        m_tcp_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, erc);
        m_tcp_socket.close();
    }

    ba::ip::tcp::resolver m_resolver;
    ba::ip::tcp::socket m_tcp_socket;
    std::vector<std::string> m_str;
};