#include "BlockProcessor.h"
#include "Logger.h"

BlockProcessor::BlockProcessor(size_t sz) : sz_(sz), m_stopped(false) {
    // std::cout << std::this_thread::get_id() << " BlockProcessor()\n";
    m_thread = std::thread(&BlockProcessor::do_work, this);
}

BlockProcessor::~BlockProcessor() {
    // std::cout << std::this_thread::get_id() << " ~BlockProcessor()\n";
    Stop();
}

void BlockProcessor::AddTask(const std::string cmd) {
    if (m_stopped) {
        throw std::runtime_error("call add task on stopped block processor");
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    tasks.push({std::move(cmd)});
    // std::cout << std::this_thread::get_id() << " BlockProcessor notify_one on update\n";
    m_cv.notify_one();
}

void BlockProcessor::Stop() {
    if (!m_stopped) {
        Logger::getLogger().log("srv: BlockProcessor stop");
        m_stopped = true;
        m_cv.notify_one();
        m_thread.join();

        if (block_level == 0 && !cmds_.empty()) {
            notify(update_events::event_print_save, {cmds_.begin(), cmds_.end()});
        }
    } else {
        // throw std::runtime_error("already stopped");
    }
}

void BlockProcessor::do_work() {
    // std::cout << std::this_thread::get_id() << " BlockProcessor do_work started " << m_stopped << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // std::cout << std::this_thread::get_id() << " BlockProcessor do_work continued " << m_stopped << std::endl;
    while (true) {
        ProcessTask task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_cv.wait(lock, [this] { return m_stopped || !tasks.empty(); });

            // std::cout << std::this_thread::get_id() << " BlockProcessor do_work awaken with " << tasks.size()
            //           << " and stopped=" << m_stopped << std::endl;

            if (m_stopped && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        Logger::getLogger().log("srv: BlockProcessor do_work RunCmds", task.cmds);

        std::istringstream iss(task.cmds);

        RunCmds(iss);
    }
}

void BlockProcessor::RunCmds(std::istream &is) {
    std::string cmd;
    size_t block_start_index = 0;

    while (std::getline(is, cmd)) {
        // std::cout << cmd << std::endl;
        if (cmd.empty())
            continue;
        else if (cmd == "{") {
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

    // std::cout << std::this_thread::get_id() << " BlockProcessor RunCmds finished\n";
}