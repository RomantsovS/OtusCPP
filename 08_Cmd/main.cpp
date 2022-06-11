#include "processing.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    if (argc < 2) throw std::runtime_error("block size is not set");

    const size_t block_size = std::stoi(argv[1]);

    BlockProcessor bp(block_size);
    bp.RunCmds(std::cin, std::cout);

    return 0;
}