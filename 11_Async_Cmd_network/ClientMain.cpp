#include <future>
#include <iostream>

#include "Client.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    ba::io_context io_context_client;
    
    std::vector<std::string> str = {"0", "1", "2", "3"};
    
    AsyncClient client(io_context_client, 7899, std::move(str));
    io_context_client.run();

    return 0;
}