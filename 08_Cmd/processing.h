#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void print_cmds(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end,
                std::ostream& os) {
    bool first = true;
    while (begin != end) {
        if (!first) os << ", ";
        os << *begin++;
        first = false;
    }
}

void print_block(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end,
                 std::ostream& os) {
    os << "bulk: ";
    print_cmds(begin, end, os);
    os << '\n';
}

void save_block(std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end) {
    auto t = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(t);

    std::ofstream myfile(std::to_string(tt));

    print_block(begin, end, myfile);
    myfile.close();
}

class BlockProcessor {
   public:
    BlockProcessor(size_t sz) : sz_(sz) {}

    void RunCmds(std::istream& is, std::ostream& os) {
        std::string cmd;
        size_t block_level = 0;
        size_t block_start_index = 0;

        while (std::getline(is, cmd)) {
            if (cmd == "{") {
                if (!cmds_.empty() && block_level == 0) {
                    print_block(std::begin(cmds_), std::end(cmds_), os);
                    save_block(std::begin(cmds_), std::end(cmds_));
                    cmds_.clear();
                }
                ++block_level;
                block_start_index = cmds_.size();
            } else if (cmd == "}") {
                if (--block_level == 0) {
                    print_block(std::begin(cmds_), std::end(cmds_), os);
                    save_block(std::begin(cmds_), std::end(cmds_));
                    cmds_.clear();
                } else {
                    save_block(std::next(std::begin(cmds_), block_start_index), std::end(cmds_));
                }
            } else {
                cmds_.push_back(std::move(cmd));
                if (block_level == 0 && cmds_.size() == sz_) {
                    print_block(std::begin(cmds_), std::end(cmds_), os);
                    save_block(std::begin(cmds_), std::end(cmds_));
                    cmds_.clear();
                }
            }
        }
        if (block_level == 0) {
            print_block(std::begin(cmds_), std::end(cmds_), os);
            save_block(std::begin(cmds_), std::end(cmds_));
        }
    }

   private:
    size_t sz_ = 1;
    std::vector<std::string> cmds_;
};
