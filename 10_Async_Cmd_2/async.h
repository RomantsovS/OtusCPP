#pragma once

#include <cstddef>
#include <list>
#include <memory>
#include <iostream>

#include "BlockPrinter.h"
#include "BlockProcessor.h"

namespace async {

struct Task {
    std::shared_ptr<BlockProcessor> bp;
    std::shared_ptr<BlockPrinter> bpr;
};

using handle_t = std::list<Task>::iterator;

handle_t connect(std::size_t bulk, std::ostream& os);
void receive(handle_t handle, const char* data, std::size_t size);
void disconnect(handle_t handle);

}  // namespace async
