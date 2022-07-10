#pragma once

#include <chrono>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "BlockPrinter.h"
#include "BlockSaver.h"

class BlockProcessor : public Observable {
   public:
    BlockProcessor(size_t sz) : sz_(sz) {}
    ~BlockProcessor() {
        // std::cout << std::this_thread::get_id() << " ~BlockProcessor()\n";
    }

    void RunCmds(std::istream& is) {
        std::string cmd;
        size_t block_level = 0;
        size_t block_start_index = 0;

        while (std::getline(is, cmd)) {
            // std::cout << cmd << std::endl;
            if (cmd == "{") {
                if (!cmds_.empty() && block_level == 0) {
                    notify(update_events::event_print_save, {cmds_.begin(), cmds_.end()});
                    cmds_.clear();
                }
                ++block_level;
                block_start_index = cmds_.size();
            } else if (cmd == "}") {
                if (--block_level == 0) {
                    notify(update_events::event_print_save, {cmds_.begin(), cmds_.end()});
                    cmds_.clear();
                } else {
                    notify(update_events::event_save, {std::next(std::begin(cmds_), block_start_index), cmds_.end()});
                }
            } else {
                cmds_.push_back(std::move(cmd));
                if (block_level == 0 && cmds_.size() == sz_) {
                    notify(update_events::event_print_save, {cmds_.begin(), cmds_.end()});
                    cmds_.clear();
                }
            }
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (block_level == 0) {
            notify(update_events::event_print_save, {cmds_.begin(), cmds_.end()});
        }

        // std::cout << std::this_thread::get_id() << " RunCmds finished\n";
    }

    void subscribe(Observer* obs) override { m_subs.push_back(obs); }

    void notify(update_events event, std::vector<std::string> cmds) override {
        for (auto s : m_subs) {
            s->update(event, cmds);
        }
    }

   private:
    size_t sz_ = 1;
    std::vector<std::string> cmds_;
    std::vector<Observer*> m_subs;
};
