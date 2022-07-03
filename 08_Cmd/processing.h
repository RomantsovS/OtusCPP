#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class update_events { event_print, event_save, event_print_save };

class Observer {
   public:
    virtual ~Observer() = default;

    virtual void update(update_events event, std::vector<std::string>::const_iterator begin,
                        std::vector<std::string>::const_iterator end) = 0;
};

class Observable {
   public:
    virtual ~Observable() = default;

    virtual void subscribe(Observer* obs) = 0;
    virtual void notify(update_events event, std::vector<std::string>::const_iterator begin,
                        std::vector<std::string>::const_iterator end) = 0;
};

class BlockPrinter : public Observer {
   public:
    BlockPrinter(Observable* bp, std::ostream& os) : m_os(os) { bp->subscribe(this); }
    BlockPrinter(std::ostream& os) : m_os(os) {}

    void update(update_events event, std::vector<std::string>::const_iterator begin,
                std::vector<std::string>::const_iterator end) override {
        if (event == update_events::event_save) return;
        m_os << "bulk: ";
        print_cmds(begin, end, m_os);
        m_os << '\n';
    }

   private:
    void print_cmds(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end,
                    std::ostream& os) {
        bool first = true;
        while (begin != end) {
            if (!first) os << ", ";
            os << *begin++;
            first = false;
        }
    }

    std::ostream& m_os;
};

class BlockSaver : public Observer {
   public:
    BlockSaver(Observable* bp) { bp->subscribe(this); }

    void update(update_events event, std::vector<std::string>::const_iterator begin,
                std::vector<std::string>::const_iterator end) override {
        if (event == update_events::event_print) return;
        save_block(begin, end);
    }

   private:
    void save_block(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end) {
        auto t = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(t);

        std::ofstream myfile(std::to_string(tt));

        BlockPrinter bp(myfile);
        bp.update(update_events::event_print, begin, end);
        myfile.close();
    }
};

class BlockProcessor : public Observable {
   public:
    BlockProcessor(size_t sz) : sz_(sz) {}

    void RunCmds(std::istream& is) {
        std::string cmd;
        size_t block_level = 0;
        size_t block_start_index = 0;

        while (std::getline(is, cmd)) {
            if (cmd == "{") {
                if (!cmds_.empty() && block_level == 0) {
                    notify(update_events::event_print_save, cmds_.begin(), cmds_.end());
                    cmds_.clear();
                }
                ++block_level;
                block_start_index = cmds_.size();
            } else if (cmd == "}") {
                if (--block_level == 0) {
                    notify(update_events::event_print_save, cmds_.begin(), cmds_.end());
                    cmds_.clear();
                } else {
                    notify(update_events::event_save, std::next(std::begin(cmds_), block_start_index), cmds_.end());
                }
            } else {
                cmds_.push_back(std::move(cmd));
                if (block_level == 0 && cmds_.size() == sz_) {
                    notify(update_events::event_print_save, cmds_.begin(), cmds_.end());
                    cmds_.clear();
                }
            }
        }
        if (block_level == 0) {
            notify(update_events::event_print_save, cmds_.begin(), cmds_.end());
        }
    }

    void subscribe(Observer* obs) override { m_subs.push_back(obs); }

    void notify(update_events event, std::vector<std::string>::const_iterator begin,
                std::vector<std::string>::const_iterator end) override {
        for (auto s : m_subs) {
            s->update(event, begin, end);
        }
    }

   private:
    size_t sz_ = 1;
    std::vector<std::string> cmds_;
    std::vector<Observer*> m_subs;
};
