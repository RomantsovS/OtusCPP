#include <future>
#include <iostream>

#include "Server.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    BlockProcessor bp(3);
    BlockPrinter bpr(std::cout, &bp);
    BlockSaver bs(&bp, 1);

    boost::asio::io_context io_context;

    Server server(io_context, 7899, bp);

    io_context.run();

    return 0;
}
