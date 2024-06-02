#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

class ThreadSafeCounter
{
    mutable std::shared_timed_mutex m_mutex;
    // std::shared_mutex - C++17
    int m_value = 0;
public:
    int get() const
    {
        std::shared_lock<std::shared_timed_mutex> lock(m_mutex);
        return m_value;
    }

    void increment()
    {
        std::unique_lock<decltype(m_mutex)> lock(m_mutex);
        ++m_value;
    }
};

int main()
{
    ThreadSafeCounter counter;

    auto increment_and_print = [&counter]() {
        for (int i = 0; i < 3; i++) {
            counter.increment();
            std::cout << counter.get() << std::endl;
        }
    };

    std::thread thread1(increment_and_print);
    std::thread thread2(increment_and_print);

    thread1.join();
    thread2.join();

    return 0;
}
