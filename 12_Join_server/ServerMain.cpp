#include "Server.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    boost::asio::io_context io_context;

    Server server(io_context, 7899);

    io_context.run();

    return 0;
}
