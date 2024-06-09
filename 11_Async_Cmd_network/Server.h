#pragma once

#include <boost/asio.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>

#include "BlockPrinter.h"
#include "BlockProcessor.h"
#include "BlockSaver.h"
#include "Logger.h"

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
    session(tcp::socket socket, BlockProcessor &bp)
        : socket_(std::move(socket)), m_bp(bp) {
        Logger::getLogger().log("srv: session started");
    }
    ~session() {
        Logger::getLogger().log("srv: session finished");
    }

    void start() { do_read(); }

private:
    void do_read() {
        Logger::getLogger().log("srv: session do_read");
        auto self(shared_from_this());
        socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    Logger::getLogger().log("srv: receive ", length, "=", std::string{data_, length});
                    m_bp.AddTask(std::string{data_, length});
                    do_read();
                } else {
                    Logger::getLogger().log("srv: session receive error:", ec.message());
                }
            });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];

    BlockProcessor &m_bp;
};

class Server {
public:
    Server(boost::asio::io_context &io_context, short port, BlockProcessor &bp)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          m_bp(bp) {
        Logger::getLogger().log("srv: Server ctor");
        do_accept();
    }
    ~Server() {
        Logger::getLogger().log("srv: Server dtor");
        boost::asio::post(acceptor_.get_executor(), [this]() {
            Logger::getLogger().log("Stopping server\n");
            acceptor_.cancel();
            stopped_ = true;
            Logger::getLogger().log("Server stopped\n");
        });
    }

private:
    void do_accept() {
        Logger::getLogger().log("srv: server do_accept");
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (ec) {
                Logger::getLogger().log("srv: accept - failed! err = ", ec.message());
                return;
            }

            Logger::getLogger().log("srv: accepted ", socket.remote_endpoint());

            if (stopped_) {
                Logger::getLogger().log("srv: Too late - the server was stopped\n");
            } else {
                std::make_shared<session>(std::move(socket), m_bp)->start();
                do_accept();
            }
        });
    }

    tcp::acceptor acceptor_;
    BlockProcessor &m_bp;
    bool stopped_ = false;
};
