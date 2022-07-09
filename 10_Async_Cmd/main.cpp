#include "processing.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    const size_t block_size = 3;

    BlockProcessor bp(block_size);
    BlockPrinter bpr(std::cout, &bp);
    BlockSaver bs(&bp, 2);

    bp.RunCmds(std::cin);

    return 0;
}