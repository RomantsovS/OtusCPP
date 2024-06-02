// Guarded suspension
// Monitor Object
// Thread pool
// Reactor
// Active Object

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <string>
#include <thread>

namespace threadSafe {

    template<typename T>
    struct Queue {

        //bool push(const T&);
        void pushWait(const T&);

        //bool pop(T &);
        void popWait(T &);

    };

}


std::condition_variable cv;
std::mutex cv_m;
std::queue<std::string> msgs;

void worker(std::queue<std::string> &q)
{
    std::unique_lock<std::mutex> lk(cv_m);
    std::cout << std::this_thread::get_id() << " waiting... " << std::endl;
    
    
    cv.wait(lk, [&q](){ return !q.empty(); });
    
    
    auto m = q.front();
    q.pop();

    std::cout << std::this_thread::get_id() << " pop " << m << std::endl;
}

int main()
{
    std::thread t1(worker, std::ref(msgs));
    std::thread t2(worker, std::ref(msgs));
    std::thread t3(worker, std::ref(msgs));
    
    {
        std::lock_guard<std::mutex> lk(cv_m);
        msgs.push("cmd1");
        msgs.push("cmd2");
    }
    cv.notify_one();

    {
        std::lock_guard<std::mutex> lk(cv_m);
        msgs.push("cmd3");
        msgs.push("cmd4");
    }
    cv.notify_one();

    t1.join();
    t2.join();
    t3.join();
    std::cout << "ok" << std::endl;

    return 0;
}
