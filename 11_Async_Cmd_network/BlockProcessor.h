#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "Observer.h"

class BlockProcessor : public Observable {
   public:
    BlockProcessor(size_t sz);
    ~BlockProcessor();

    void AddTask(const std::string cmd);
    void Stop();

    void RunCmds(std::istream& is);

    void subscribe(Observer* obs) override { m_subs.push_back(obs); }

    void notify(update_events event, std::vector<std::string> cmds) override {
        for (auto s : m_subs) {
            s->update(event, cmds);
        }
    }

   private:
    void do_work();

    size_t sz_ = 1;
    std::vector<std::string> cmds_;
    size_t block_level = 0;
    std::vector<Observer*> m_subs;

    std::mutex m_mutex;
    std::condition_variable m_cv;

    struct ProcessTask {
        std::string cmds;
    };

    std::queue<ProcessTask> tasks;
    std::atomic<bool> m_stopped;
    std::thread m_thread;
};
