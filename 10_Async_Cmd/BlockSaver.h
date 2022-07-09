#pragma once

#include <string>
#include <vector>

#include "BlockPrinter.h"
#include "Observer.h"

class BlockSaver : public Observer {
   public:
    BlockSaver(Observable* bp, size_t threads);
    ~BlockSaver();

    void update(update_events event, std::vector<std::string> cmds) override;

    void Stop();

   private:
    void do_work();

    void save_block(std::vector<std::string> cmds);

    struct SaveTask {
        std::vector<std::string> cmds;
    };

    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::queue<SaveTask> tasks;
    std::vector<std::thread> m_threads;
    std::atomic<bool> m_stopped;
};