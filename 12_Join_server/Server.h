#pragma once

#include <algorithm>
#include <boost/asio.hpp>
#include <cstddef>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>

#include "DB.h"

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
   public:
    session(tcp::socket socket) : socket_(std::move(socket)) {
        // std::cout << "session started\n";
    }

    void start() {
        db_ptr = std::make_unique<DB>();
        do_read();
    }

   private:
    void do_read() {
        // std::cout << "session do_read\n";
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        // std::cout << "receive " << length << "=" <<
                                        // std::string{data_, length} << std::endl;
                                        auto qr = db_ptr->Exec(std::string{data_, length});
                                        auto str = qr->AsString();
                                        str.copy(data_, str.size());
                                        do_write(str.size());
                                    } else {
                                        // std::cout << "session receive error:" << ec.message() <<
                                        // std::endl;
                                    }
                                });
    }

    void do_write(size_t length) {
        auto self(shared_from_this());

        boost::asio::async_write(
            socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                }
            });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    std::unique_ptr<DB> db_ptr;
    std::vector<std::string> commands = {"INSERT", "SELECT"};
};

class Server {
   public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
    // ~Server() { std::cout << "Server dtor\n"; }

   private:
    void do_accept() {
        std::cout << "server do_accept\n";
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<session>(std::move(socket))->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
};
