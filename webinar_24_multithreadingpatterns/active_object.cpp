#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <thread>
#include <queue>

struct ActiveObject {
	using TTask = std::function<void()>;
	using TCallback = std::function<void()>;

	ActiveObject()
		: m_stopped{false} {
		m_thread = std::thread(&ActiveObject::workerThread, this);
	}

	void doSomething(const TCallback& callback) {
		std::lock_guard<std::mutex> guard(m_mutex);
		auto task = [this](){ internalDoSomething(); };
		m_tasks.push(
			std::make_pair(
				task,
				callback
			)
		);
		m_condition.notify_one();

	}

	void stop() {
		if (!m_stopped) {
			m_stopped = true;
			m_condition.notify_one();
			m_thread.join();
		}
	}

private:
	void internalDoSomething() {
		std::cout << "Hello from internalDoSomething" << std::endl;
	}

	void workerThread() {
		std::cout << "workerThread started!" << std::endl;
		
		while (!m_stopped) {
			std::unique_lock<std::mutex> lck{m_mutex};
			while (!m_stopped && m_tasks.empty())
				m_condition.wait(lck);

			if (m_stopped)
				break;

			auto pair = m_tasks.front();
			m_tasks.pop();
            pair.first();
            pair.second();
		}

        // Small workaround
		while(!m_tasks.empty()) {
			auto pair = m_tasks.front();
			m_tasks.pop();
			pair.first();
			pair.second();
		}

		std::cout << "workerThread stopped!" << std::endl;
	}

	std::condition_variable m_condition;
	std::queue<std::pair<TTask, TCallback>> m_tasks;
	std::mutex m_mutex;
	std::thread m_thread;
	std::atomic<bool> m_stopped;

};


int main() {

	ActiveObject activeObject;

	auto callback = []() {
		std::cout << "Hello from callback!" << std::endl;
	};

	activeObject.doSomething(callback);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	activeObject.stop();

	return 0;
}
