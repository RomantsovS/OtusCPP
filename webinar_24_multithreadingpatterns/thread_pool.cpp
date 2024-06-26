#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <vector>

thread_local int val;

class ThreadPool {
   public:
    ThreadPool(size_t);
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    // auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

    ~ThreadPool();

   private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;

    // the task queue
    std::queue<std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;

    std::condition_variable condition;

    bool stop;
};

// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back([this] {
            std::cout << "worker sleep\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout << "worker awake\n";
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);

                    this->condition.wait(lock, [this] {
                        std::cout << "test\n";
                        return this->stop || !this->tasks.empty();
                    });

                    if (this->stop && this->tasks.empty()) return;

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                task();
            }
        });
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f,
                         Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()> >(

        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool

        if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); });
        std::stringstream ss;
        ss << "tasks: " << tasks.size() << std::endl;
        std::cout << ss.str();
    }

    condition.notify_one();

    return res;
}

// the destructor joins all threads
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
        std::cout << "stop\n";
    }

    condition.notify_all();

    for (std::thread& worker : workers) worker.join();
}

int main()

{
    std::vector<std::future<int> > results;

    ThreadPool pool(1);

    for (int i = 0; i < 50; ++i) {
        results.emplace_back(

            pool.enqueue([i]

                         {
                             {
                                 std::stringstream ss;
                                 ss << "hello " << i << std::endl;
                                 std::cout << ss.str();
                             }

                             std::this_thread::sleep_for(std::chrono::seconds(1));

                             {
                                 std::stringstream ss;
                                 ss << "world " << i << std::endl;
                                 std::cout << ss.str();
                             }

                             return i * i;
                         }));
    }

    std::this_thread::sleep_for(std::chrono::seconds(100));
    std::cout << "Results..." << std::endl;

    for (auto&& result : results) std::cout << result.get() << ' ';

    std::cout << std::endl;

    return 0;
}
