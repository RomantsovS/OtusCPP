#pragma once

#include <array>
#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "Logger.h"

namespace ba = boost::asio;

class AsyncClient {
public:
    AsyncClient(ba::io_context &io_context, short port, std::vector<std::string> str)
        : m_resolver{io_context},
          m_tcp_socket{io_context},
          m_str(std::move(str)) {
        ba::ip::tcp::endpoint ep(ba::ip::address::from_string("127.0.0.1"), port);

        m_tcp_socket.async_connect(ep, [&](const boost::system::error_code &erc) {
            connectHandler(erc);
        });
    }

private:
    void connectHandler(const boost::system::error_code &ec) {
        Logger::getLogger().log("cli: connectHandler");
        if (ec) {
            Logger::getLogger().log("cli: connectHandler - failed! err = ", ec.message());
            return;
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));

        for (auto &str : m_str) {
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            Logger::getLogger().log("cli: send ", str);
            ba::write(m_tcp_socket, ba::buffer(str));
        }
        boost::system::error_code erc;
        m_tcp_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, erc);
        m_tcp_socket.close();
    }

    ba::ip::tcp::resolver m_resolver;
    ba::ip::tcp::socket m_tcp_socket;
    std::vector<std::string> m_str;
};