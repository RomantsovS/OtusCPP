#pragma once

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "Observer.h"

class BlockPrinter : public Observer {
   public:
    BlockPrinter(std::ostream& os, Observable* bp = nullptr);
    ~BlockPrinter();

    void update(update_events event, std::vector<std::string> cmds) override;

    void Stop();

   private:
    void do_work();

    void print_cmds(const std::vector<std::string>& cmds) const;

    std::ostream& m_os;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    struct PrintTask {
        std::vector<std::string> cmds;
    };

    std::queue<PrintTask> tasks;
    std::atomic<bool> m_stopped;
    std::thread m_thread;
};