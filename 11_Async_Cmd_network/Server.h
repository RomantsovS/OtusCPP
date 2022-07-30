#pragma once

#include <boost/asio.hpp>
#include <cstddef>
#include <iostream>
#include <list>
#include <memory>

#include "BlockPrinter.h"
#include "BlockProcessor.h"
#include "BlockSaver.h"

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
   public:
    session(tcp::socket socket, BlockProcessor& bp)
        : socket_(std::move(socket)), m_bp(bp) {
        // std::cout << "session started\n";
    }

    void start() { do_read(); }

   private:
    void do_read() {
        // std::cout << "session do_read\n";
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        // std::cout << "receive " << length << "=" <<
                                        // std::string{data_, length} << std::endl;
                                        m_bp.AddTask(std::string{data_, length});
                                        do_read();
                                    } else {
                                        // std::cout << "session receive error:" << ec.message() <<
                                        // std::endl;
                                    }
                                });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];

    BlockProcessor& m_bp;
};

class Server {
   public:
    Server(boost::asio::io_context& io_context, short port, BlockProcessor& bp)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), m_bp(bp) {
        do_accept();
    }
    // ~Server() { std::cout << "Server dtor\n"; }

   private:
    void do_accept() {
        // std::cout << "server do_accept\n";
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<session>(std::move(socket), m_bp)->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;

    BlockProcessor& m_bp;
};
