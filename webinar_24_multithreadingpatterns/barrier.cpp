#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

class Barrier {
   private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    std::atomic<int> m_count{0};

   public:
    explicit Barrier(const unsigned initialCount);
    void Wait();
};

Barrier::Barrier(const unsigned initialCount) : m_count(initialCount) {}

void Barrier::Wait() {
    unique_lock<mutex> mtxLock(m_mtx);  // Must use unique_lock with condition variable.
    --m_count;
    if (0 == m_count) {
        m_cv.notify_all();
    } else {
        m_cv.wait(mtxLock, [this] { return 0 == m_count; });  // Blocking till count is zero.
    }
}

const int TotalThreads = 5;
std::mutex printMtx;
static Barrier barrier(TotalThreads);

static void RunAtSameTime(int index) {
    std::cout << std::this_thread::get_id() << " start at "
              << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
    barrier.Wait();  // Comment this line out for Without barrier test.
    unique_lock<mutex> mtxLock(printMtx);
    std::cout << index << " run at "
              << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
}

int main() {
    std::thread threads[TotalThreads];
    std::cout << "Start all threads.\n";
    for (int i = 0; i < TotalThreads; ++i) {
        threads[i] = std::thread(RunAtSameTime, i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    for (auto& th : threads) th.join();
}
