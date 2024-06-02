// Fission / Nuclear reaction

#include <algorithm>
#include <atomic>
#include <iostream>
#include <functional>
#include <thread>
#include <vector>

constexpr size_t ValuesCount = 1000;

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& values) {
	for (auto& v : values)
		os << v << ' ';
	return os;
}

namespace motivation {
	void example() {
		std::cout << "\nmotivation example\n";

		std::vector<int> values(ValuesCount, 0);
		std::size_t sum = 0;

		std::for_each(
			values.begin(),
			values.end(),
			[&sum](int& value) {
				sum += value + 42;
			}
		);

		std::cout << "Result sum = " << sum << std::endl;
		std::cout << std::endl;
	}
}

namespace basics {

	void nuclearReaction() {
		std::cout << "\nbasics nuclearReaction\n";

		using Values_t = std::vector<int>;

		Values_t values(ValuesCount, 0);
        std::atomic<std::size_t> sum = {0};

		auto worker = [](const Values_t::iterator& from, const Values_t::iterator& to) {
			std::size_t localSum = 0;
			std::for_each(from, to,
				[&localSum](int& value) {
					localSum += value + 42;
				}
			);
			return localSum;
		};

		const int maxThreadCount = 8;
		std::atomic<int> threadCount{1};

		std::function<void(const Values_t::iterator&, const Values_t::iterator&)> nuclearWorker;
		
		nuclearWorker = [&](
			const Values_t::iterator& from, 
			const Values_t::iterator& to
			) {

				auto size = std::distance(from, to);

				if (threadCount > maxThreadCount || size < 10) {
					std::cout << std::this_thread::get_id()
						<< " Hello from next thread! I will take " << size << " elements to work" 
						<< std::endl;
					sum += worker(from, to);
				}
				else {
					auto middle = std::next(from, size / 2);
					threadCount += 2;
					std::thread worker1 = std::thread(nuclearWorker, from, middle);
					std::thread worker2 = std::thread(nuclearWorker, middle, to);
					worker1.join();
					worker2.join();
				}
			};


		// Start nuclear reaction
		// std::thread first = std::thread(nuclearWorker, values.begin(), values.end());
		// first.join();
		nuclearWorker(values.begin(), values.end());

		std::cout << "Result sum = " << sum << std::endl;
		std::cout << std::endl;
	}

}

namespace middle {
	void nuclearReaction() {
		std::cout << "\nmiddle nuclearReaction\n";

		using Values_t = std::vector<int>;

		Values_t values(ValuesCount, 0);
        std::atomic<std::size_t> sum = {0};

		auto worker = [](const Values_t::iterator& from, const Values_t::iterator& to) {
			std::size_t localSum = 0;
			std::for_each(from, to,
				[&localSum](int& value) {
					localSum += value + 42;
				}
			);
			return localSum;
		};

		const int maxThreadCount = 8;
		std::atomic<int> threadCount{1};

		std::function<void(const Values_t::iterator&, const Values_t::iterator&)> nuclearWorker;
		
		nuclearWorker = [&](
			const Values_t::iterator& from, 
			const Values_t::iterator& to
			) {

				auto size = std::distance(from, to);
				auto selfPart = size / maxThreadCount;

				if (threadCount > maxThreadCount || size < 10) {
					std::cout << std::this_thread::get_id()
						<< " Hello from next thread! I will take " << size << " elements to work " 
						<< std::endl;
					sum += worker(from, to);
				}
				else {
					std::cout << std::this_thread::get_id()
						<< "Hello from next thread! I will take " << selfPart << " elements to work "
						"and pass " << size - selfPart << " elements to child threads" << std::endl;

					auto selfFrom = from;
					auto selfTo = std::next(selfFrom, selfPart);

					auto nextFrom = selfTo;
					auto nextTo = to;
					auto nextSize = size - selfPart;

					auto middle = std::next(nextFrom, nextSize / 2);
					threadCount += 2;
					std::thread worker1 = std::thread(nuclearWorker, nextFrom, middle);
					std::thread worker2 = std::thread(nuclearWorker, middle, nextTo);

					sum += worker(selfFrom, selfTo);
					worker1.join();
					worker2.join();
				}
			};

		nuclearWorker(values.begin(), values.end());

		std::cout << "Result sum = " << sum << std::endl;
		std::cout << std::endl;
	}
}

int main() {

	motivation::example();
	basics::nuclearReaction();
	middle::nuclearReaction();

	return 0;
}
