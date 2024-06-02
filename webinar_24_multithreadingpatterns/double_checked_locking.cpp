#include <iostream>
#include <mutex>
#include <thread>

namespace singleThread {
	struct Singleton {
		static Singleton* getInstance();

		void doSomething();
	private:
		static Singleton* m_instance;
	};

	Singleton* Singleton::m_instance = nullptr;

	/*static*/ Singleton* Singleton::getInstance() {
		
		if (!m_instance)
			m_instance = new Singleton{};
		
		return m_instance;
	}

	void Singleton::doSomething() {
		std::cout << "Hello from singleThread Singleton!" << std::endl;
	}
}

namespace naiveImpl {
	struct Singleton {
		static Singleton* getInstance();

		void doSomething();
	private:
		static Singleton* m_instance;
		static std::mutex m_mutex;
	};

	Singleton* Singleton::m_instance = nullptr;
	std::mutex Singleton::m_mutex;

	Singleton* Singleton::getInstance() {

		std::lock_guard<std::mutex> lck{m_mutex};
		
		if (!m_instance)
			m_instance = new Singleton{};
		
		return m_instance;
	}

	void Singleton::doSomething() {
			std::cout << "Hello from naiveImpl Singleton!" << std::endl;
	}
}


namespace doubleCheckedLock {
	struct Singleton {
		static Singleton* getInstance();

		void doSomething();
	private:
		static Singleton* m_instance;
		static std::mutex m_mutex;
	};

	Singleton* Singleton::m_instance = nullptr;
	std::mutex Singleton::m_mutex;

	Singleton* Singleton::getInstance() {

		if (!m_instance) {
			std::lock_guard<std::mutex> lck{m_mutex};
		
			if (!m_instance)
            {
				m_instance = new Singleton{};
            }
		}

		return m_instance;
	}

	void Singleton::doSomething() {
			std::cout << "Hello from doubleCheckedLock Singleton!" << std::endl;
	}
}

namespace callOnce {
	struct Singleton {
		static Singleton* getInstance();

		void doSomething();
	private:
		static void create();

		static Singleton* m_instance;
		static std::once_flag m_onceFlag;
	};

	Singleton* Singleton::m_instance = nullptr;
	std::once_flag Singleton::m_onceFlag;

	Singleton* Singleton::getInstance() {
		std::call_once(m_onceFlag, Singleton::create);
		return m_instance;
	}

	void Singleton::create() {
		// What if exception?
		m_instance = new Singleton{};
	}

	void Singleton::doSomething() {
		std::cout << "Hello from callOnce Singleton!" << std::endl;
	}
}

namespace justStatic {
	struct Singleton {
		static Singleton* getInstance() {
			static Singleton* instance = new Singleton();
			return instance;
		}

		void doSomething() {
			std::cout << "Hello from justStatic Singleton!" << std::endl;
		}
	};
}

int main() {
    //Singleton::getInstance();
	singleThread::Singleton::getInstance()->doSomething();
	naiveImpl::Singleton::getInstance()->doSomething();
	doubleCheckedLock::Singleton::getInstance()->doSomething();

	callOnce::Singleton::getInstance()->doSomething();

	justStatic::Singleton::getInstance()->doSomething();

	return 0;
}
