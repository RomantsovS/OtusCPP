#include <cstddef>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class Vector {
   public:
    Vector() = default;

    Vector(size_t n, const Allocator& alloc = Allocator()) : allocator(alloc) {
        data = allocator.allocate(n);

        for (size_t i = 0; i < n; ++i) {
            allocator.construct(data + i);
        }

        sz = cp = n;
    }

    Vector(const Vector& other) {
        data = allocator.allocate(other.sz);

        for (size_t i = 0; i < other.sz; ++i) {
            allocator.construct(data + i, other.data[i]);
        }

        sz = cp = other.sz;
    }

    ~Vector() = default;

    void reserve(size_t n) {
        if (n > cp) {
            auto data2 = allocator.allocate(n);
            for (size_t i = 0; i < sz; ++i) {
                allocator.construct(data2 + i, data[i]);
            }
            for (size_t i = 0; i < cp; ++i) {
                allocator.destroy(data + i);
            }
            data = data2;
            cp = n;
        }
    }

    void resize(size_t n) {
        reserve(n);
        if (sz < n) {
            for (size_t i = sz; i < n; ++i) {
                allocator.construct(data + i, T());
            }
        } else if (sz > n) {
            for (size_t i = sz; i >= n; --i) {
                allocator.destroy(data + i - 1);
            }
        }
        sz = n;
    }

    void push_back(const T& elem) {
        if (sz == cp) {
            reserve(sz == 0 ? 1 : sz * 2);
        }
        allocator.construct(data + sz, elem);
        ++sz;
    }

    size_t size() const noexcept { return sz; }

    size_t capacity() const noexcept { return cp; }

    T& operator[](size_t i) { return *(data + i); }
    const T& operator[](size_t i) const { return *(data + i); }

    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept { return data; }
    iterator end() noexcept { return data + sz; }

    const_iterator begin() const noexcept { return data; }
    const_iterator end() const noexcept { return data + sz; }

    const_iterator cbegin() const noexcept { return data; }

    const_iterator cend() const noexcept { return data + sz; }

   private:
    Allocator allocator;
    T* data = nullptr;
    size_t sz = 0;
    size_t cp = 0;
};
