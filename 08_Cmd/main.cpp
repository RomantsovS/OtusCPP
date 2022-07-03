#include "processing.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    if (argc < 2) throw std::runtime_error("block size is not set");

    const size_t block_size = std::stoi(argv[1]);

    BlockProcessor bp(block_size);
    BlockPrinter bpr(&bp, std::cout);
    BlockSaver bs(&bp);

    bp.RunCmds(std::cin);

    return 0;
}