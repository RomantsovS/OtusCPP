#pragma once

#include <fstream>
#include <iostream>
#include <mutex>

class Logger {
public:
    static Logger &getLogger() {
        static Logger logger;
        return logger;
    }

    template <typename... Args>
    void log(Args... args) {
        std::unique_lock ul(mtx);
        ofs << "thread id: " << std::this_thread::get_id();
        ((ofs << " " << std::forward<Args>(args)), ...);
        ofs << std::endl;
    }

private:
    Logger() : ofs("log.log", std::ios::out | std::ios::app) {
        std::cout << __PRETTY_FUNCTION__ << std ::endl;
    }
    std::ofstream ofs;
    std::mutex mtx;
};
