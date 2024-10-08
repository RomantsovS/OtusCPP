#include "BlockPrinter.h"
#include "Logger.h"

BlockPrinter::BlockPrinter(std::ostream &os, Observable *bp) : m_os(os), m_stopped(false) {
    // std::cout << std::this_thread::get_id() << " BlockPrinter()\n";
    m_thread = std::thread(&BlockPrinter::do_work, this);
    if (bp) {
        bp->subscribe(this);
    }
}

BlockPrinter::~BlockPrinter() {
    // std::cout << std::this_thread::get_id() << " ~BlockPrinter()\n";
    Stop();
}

void BlockPrinter::update(update_events event, std::vector<std::string> cmds) {
    if (event == update_events::event_save)
        return;

    std::unique_lock<std::mutex> lock(m_mutex);
    tasks.push({std::move(cmds)});
    // std::cout << std::this_thread::get_id() << " BlockPrinter notify_one on update\n";
    m_cv.notify_one();
}

void BlockPrinter::Stop() {
    if (!m_stopped) {
        Logger::getLogger().log("srv: BlockPrinter stop");
        m_stopped = true;
        m_cv.notify_one();
        m_thread.join();
    } else {
        // throw std::runtime_error("already stopped");
    }
}

void BlockPrinter::do_work() {
    // std::cout << std::this_thread::get_id() << " BlockPrinter do_work started " << m_stopped << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // std::cout << std::this_thread::get_id() << " BlockPrinter do_work continued " << m_stopped << std::endl;
    while (true) {
        PrintTask task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_cv.wait(lock, [this] { return m_stopped || !tasks.empty(); });

            // std::cout << std::this_thread::get_id() << " BlockPrinter do_work awaken with " << tasks.size()
            //           << " and stopped=" << m_stopped << std::endl;

            if (m_stopped && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        Logger::getLogger().log("srv: BlockPrinter do_work print_cmds size:", task.cmds.size());

        m_os << "bulk: ";
        print_cmds(task.cmds);
        m_os << '\n';
    }
}

void BlockPrinter::print_cmds(const std::vector<std::string> &cmds) const {
    bool first = true;
    for (const auto &cmd : cmds) {
        if (!first)
            m_os << ", ";
        m_os << cmd;
        first = false;
    }
}