#include "BlockSaver.h"

#include <chrono>
#include <fstream>
#include <sstream>

BlockSaver::BlockSaver(Observable* bp, size_t threads) : m_stopped(false) {
    // std::cout << std::this_thread::get_id() << " BlockSaver()\n";
    bp->subscribe(this);
    for (size_t i = 0; i < threads; ++i) {
        m_threads.emplace_back(&BlockSaver::do_work, this);
    }
}

BlockSaver::~BlockSaver() {
    // std::cout << std::this_thread::get_id() << " ~BlockSaver()\n";
    Stop();
}

void BlockSaver::update(update_events event, std::vector<std::string> cmds) {
    if (event == update_events::event_print) return;
    std::unique_lock<std::mutex> lock(m_mutex);
    tasks.push({std::move(cmds)});
    m_cv.notify_one();
}

void BlockSaver::Stop() {
    if (!m_stopped) {
        m_stopped = true;
        m_cv.notify_all();

        for (auto& thread : m_threads) {
            thread.join();
        }
    }
}

void BlockSaver::do_work() {
    // std::cout << std::this_thread::get_id() << " BlockSaver do_work started\n";
    while (true) {
        SaveTask task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_cv.wait(lock, [this] { return m_stopped || !tasks.empty(); });

            if (m_stopped && tasks.empty()) return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        // std::cout << std::this_thread::get_id() << " do_work save_block\n";

        save_block(std::move(task.cmds));
    }
}

void BlockSaver::save_block(std::vector<std::string> cmds) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    auto t = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(t);

    std::ostringstream oss;
    oss << tt << "-" << std::this_thread::get_id();

    std::ofstream myfile(oss.str());
    if(!myfile)
        throw std::runtime_error("file open error");

    BlockPrinter bp(myfile);
    bp.update(update_events::event_print, std::move(cmds));
    bp.Stop();
    
    // std::cout << std::this_thread::get_id() << " file closed " << oss.str() << std::endl;

    myfile.close();
}