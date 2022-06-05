#include <list>
#include <tuple>
#include <utility>

template <typename T, typename Container>
class IteratorRow;

template <typename T, typename Container>
class Iterator;

template <typename T, T DEF_VAL>
class Matrix {
    friend class IteratorRow<T, Matrix>;
    friend class Iterator<T, Matrix>;

   public:
    static const T def_val = DEF_VAL;

    size_t size() const { return cur_size; }
    Iterator<T, Matrix> begin() {
        if (!rows.empty()) {
            auto it = rows.begin();

            return Iterator<T, Matrix>{this, it, it->elems.begin()};
        }
        return end();
    }

    Iterator<T, Matrix> end() { return Iterator<T, Matrix>{this, rows.end()}; }

    IteratorRow<T, Matrix> operator[](size_t row_num) {
        for (auto& row : rows) {
            if (row.row_num_ == row_num) {
                return IteratorRow<T, Matrix>{this, row_num, &row};
            }
        }
        return {this, row_num};
    }

   private:
    struct Element {
        T item;
        size_t col_num = 0;
    };
    struct Row {
        Row(size_t row_num) : row_num_(row_num) {}

        size_t row_num_ = 0;
        std::list<Element> elems;
        using elem_iter_type = typename std::list<Element>::iterator;
    };

    size_t cur_size = 0;

    std::list<Row> rows;
    using rows_iter_type = typename std::list<Row>::iterator;
};

template <typename T, typename Container>
class IteratorRow {
    friend class Matrix<T, Container::def_val>;

   public:
    T operator[](size_t col_num) const {
        if (row_ptr_) {
            for (auto& elem : row_ptr_->elems) {
                if (elem.col_num == col_num) {
                    return elem.item;
                }
            }
        }
        return Container::def_val;
    }

    struct IndexProxy {
        IndexProxy(IteratorRow& row_iter, size_t col) : row_iter_(row_iter), col_(col) {}

        IndexProxy& operator=(T val) {
            bool is_def_val = val == row_iter_.container_->def_val;

            if (!row_iter_.row_ptr_) {
                if (!is_def_val) {
                    row_iter_.AddRow()->elems.push_back({val, col_});
                    ++row_iter_.container_->cur_size;
                }
            } else {
                auto iter = row_iter_.row_ptr_->elems.begin();
                while (iter != row_iter_.row_ptr_->elems.end() && iter->col_num < col_) iter = std::next(iter);

                if (iter != row_iter_.row_ptr_->elems.end() && iter->col_num == col_) {
                    if(is_def_val) {
                        row_iter_.row_ptr_->elems.erase(iter);
                        --row_iter_.container_->cur_size;
                    } else
                        iter->item = val;
                } else if(!is_def_val) {
                    row_iter_.row_ptr_->elems.insert(iter, {val, col_});
                    ++row_iter_.container_->cur_size;
                }
            }

            return *this;
        }

        operator T() const { return std::as_const(row_iter_)[col_]; }

        IteratorRow& row_iter_;
        size_t col_;
    };

    IndexProxy operator[](size_t col) { return {*this, col}; }

   private:
    IteratorRow(Container* container, size_t row_num, typename Container::Row* row_ptr = nullptr)
        : container_(container), row_num_(row_num), row_ptr_(row_ptr) {}

    typename Container::Row* AddRow() {
        if (container_->rows.empty()) {
            container_->rows.push_back({row_num_});
            row_ptr_ = &container_->rows.back();
        } else {
            auto iter = container_->rows.begin();
            while (iter != container_->rows.end() && iter->row_num_ < row_num_) iter = std::next(iter);

            if (iter != container_->rows.end() && iter->row_num_ == row_num_) {
                assert(0);
            } else {
                row_ptr_ = &*container_->rows.insert(iter, {row_num_});
            }
        }
        return row_ptr_;
    }
    Container* container_ = nullptr;
    size_t row_num_ = 0;
    typename Container::Row* row_ptr_ = nullptr;
};

template <typename T, typename Container>
class Iterator {
    friend class Matrix<T, Container::def_val>;

   public:
    bool operator!=(const Iterator<T, Container>& other) const { return row_iter_ != other.row_iter_; }

    Iterator& operator++() {
        elem_iter_ = std::next(elem_iter_);
        if (elem_iter_ == row_iter_->elems.end()) {
            ++row_iter_;
            if (row_iter_ != container_->rows.end()) elem_iter_ = row_iter_->elems.begin();
        }
        return *this;
    }

    Iterator operator++(int) {
        auto res = *this;

        operator++();

        return res;
    }

    std::tuple<size_t, size_t, T> operator*() {
        return std::make_tuple(row_iter_->row_num_, elem_iter_->col_num, elem_iter_->item);
    }

   private:
    Iterator(Container* container, typename Container::rows_iter_type row_iter,
             typename Container::Row::elem_iter_type elem_iter = typename Container::Row::elem_iter_type())
        : container_(container), row_iter_(row_iter), elem_iter_(elem_iter) {}

    Container* container_ = nullptr;
    typename Container::rows_iter_type row_iter_;
    typename Container::Row::elem_iter_type elem_iter_;
};