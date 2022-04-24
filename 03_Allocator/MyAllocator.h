#pragma once

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <vector>

#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define UNUSED(expr)  \
    do {              \
        (void)(expr); \
    } while (0)

template <typename T, size_t _block_size, bool allowAllocks = false>
class logging_allocator {
public:
	using value_type = T;

	template <typename U>
	struct rebind {
		using other = logging_allocator<U, _block_size, allowAllocks>;
	};

	logging_allocator(bool free_on_deallocate = false) : free_on_deallocate_(free_on_deallocate) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}
	~logging_allocator() { ShutDown(); }

	template <typename U>
	logging_allocator(const logging_allocator<U, _block_size, allowAllocks>&) : free_on_deallocate_(true) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	T* allocate(std::size_t n);

	void deallocate(T* p, std::size_t n);

	template <typename U, typename... Args>
	void construct(U* p, Args &&...args);

	void destroy(T* p);

	size_t GetTotalCount() const { return total; }
	size_t GetAllocCount() const { return active; }

private:
	void AllocNewBlock();
	void ShutDown();

	std::allocator<T> allocator;

	struct Block {
		T* data;
		size_t count;
	};

	std::vector<Block> blocks;
	T* free = nullptr;
	size_t active = 0;
	size_t total = 0;
	bool free_on_deallocate_ = false;
};

template <typename T, size_t _block_size, bool allowAllocks>
T* logging_allocator<T, _block_size, allowAllocks>::allocate(std::size_t n) {
	std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;

	T* res = nullptr;

	if (!free_on_deallocate_) {
		if (n > _block_size)
			throw std::runtime_error("too big data size");

		if (blocks.empty()) {
			AllocNewBlock();
		} else if(allowAllocks) {
			if (blocks.back().count >= _block_size) {					
				AllocNewBlock();
			}
		} else if(active + n > _block_size)
			throw std::runtime_error("allocator is full");

		blocks.back().count += n;

		res = free;
		free += n;
	}
	else {
		free = allocator.allocate(n);

		std::cout << "allocated " << n << " * " << sizeof(T) << " bytes at: " << free << std::endl;
		std::cout << 0 << " allocated at: " << free << std::endl;

		res = free;
	}

	active += n;
	std::cout << "active: " << active << " free at: " << free << std::endl;

	return res;
}

template <typename T, size_t _block_size, bool allowAllocks>
void logging_allocator<T, _block_size, allowAllocks>::deallocate(T* p, std::size_t n) {
	std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;

	if (free_on_deallocate_) {
		allocator.deallocate(p, n);
		std::cout << "freed at: " << p << std::endl;
	}
}

template <typename T, size_t _block_size, bool allowAllocks>
template <typename U, typename... Args>
void logging_allocator<T, _block_size, allowAllocks>::construct(U* p, Args &&...args) {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	std::cout << "constructed at: " << p << std::endl;
	new (p) U(std::forward<Args>(args)...);
}

template <typename T, size_t _block_size, bool allowAllocks>
void logging_allocator<T, _block_size, allowAllocks>::destroy(T* p) {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	std::cout << "destructed at: " << p << std::endl;
	p->~T();
	}

template <typename T, size_t _block_size, bool allowAllocks>
void logging_allocator<T, _block_size, allowAllocks>::AllocNewBlock() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	T* ptr = allocator.allocate(_block_size);
	if (!ptr) throw std::bad_alloc();

	blocks.push_back({ ptr, 0 });

	std::cout << "allocated " << _block_size << " * " << sizeof(T) << " bytes at: " << ptr << std::endl;
	for (size_t i = 0; i < _block_size; ++i) {
		std::cout << i << " allocated at: " << (ptr + i) << std::endl;
	}

	free = ptr;

	total += _block_size;
}

template <typename T, size_t _block_size, bool allowAllocks>
void logging_allocator<T, _block_size, allowAllocks>::ShutDown() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	if (!free_on_deallocate_) {
		for (auto iter = blocks.rbegin(); iter != blocks.rend(); ++iter) {
			allocator.deallocate(iter->data, iter->count);
			std::cout << "freed at: " << iter->data << std::endl;
		}
	}
}
