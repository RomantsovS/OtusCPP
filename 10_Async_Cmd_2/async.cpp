#include "async.h"

namespace async {

std::list<Task> proc_list;

handle_t connect(std::size_t bulk, std::ostream& os) {
    auto bp = std::make_shared<BlockProcessor>(bulk);
    auto bpr = std::make_shared<BlockPrinter>(os, bp.get());

    proc_list.push_back({std::move(bp), std::move(bpr)});

    return std::prev(proc_list.end());
}

void receive(handle_t handle, const char* data, std::size_t size) { handle->bp->AddTask(std::string(data, size)); }

void disconnect(handle_t handle) {
    handle->bp->Stop();
    proc_list.erase(handle);
}

}  // namespace async
