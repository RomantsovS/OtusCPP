#pragma once

#include <iostream>
#include <tuple>

#include "mapreduce.h"

struct Data {
    std::string line;
    int duplicates;
    size_t max_length;
};

std::ostream& operator<<(std::ostream& os, const Data& data) {
    os << data.line << " " << data.duplicates << " " << data.max_length;
    return os;
}

std::istream& operator>>(std::istream& is, Data& data) {
    is >> data.line >> data.duplicates >> data.max_length;
    return is;
}

bool operator<(const Data& lhs, const Data& rhs) {
    return lhs.line < rhs.line;
}

bool operator==(const Data& lhs, const Data& rhs) {
    return lhs.line == rhs.line;
}

bool operator!=(const Data& lhs, const Data& rhs) {
    return !(lhs == rhs);
}

size_t calc_min_prefix(std::istream& is, const std::filesystem::path& output,
size_t mappers_count, size_t reducers_count) {
    MapReduce<Data> mr(mappers_count, reducers_count);

    size_t prefix_size = 1;

    bool run = true;

    while (run) {
        mr.set_mapper([prefix_size](const std::string& line) -> auto{
            // моё предложение:
            //     * получает строку,
            //     * выделяет префикс,
            //     * возвращает пары (префикс, 1).
            return Data{line.substr(0, prefix_size), 1, line.size()};
        });

        mr.set_reducer([](Data res, Data cur) -> auto{
            // моё предложение:
            //     * получает пару (префикс, число),
            //     * если текущий префикс совпадает с предыдущим или имеет число > 1, то возвращает
            //     false,
            //     * иначе возвращает true.
            //
            // Почему тут написано "число", а не "1"?
            // Чтобы учесть возможность добавления фазы combine на выходе мапера.
            // Почитайте, что такое фаза combine в hadoop.
            // Попробуйте это реализовать, если останется время.
            
            if (res.line == cur.line) {
                ++res.duplicates;
                return res;
            } else if (res.duplicates > 1)
                return res;
            return cur;
        });

        mr.run(is, output);

        bool has_duplicates = false;

        for (auto const& dir_entry : std::filesystem::directory_iterator{output}) {
            if (!std::filesystem::is_directory(dir_entry.path())) {
                std::ifstream fin(dir_entry.path());
                std::string line;
                size_t num_duplicates, max_len;
                fin >> line >> num_duplicates >> max_len;
                if (max_len <= prefix_size) {
                    run = false;
                    break;
                }
                if(num_duplicates > 1) {
                    has_duplicates = true;
                    break;
                }
            }
        }

        if(!has_duplicates)
            run = false;

        ++prefix_size;

        is.clear();
        is.seekg(0, std::ios_base::beg);
    }

    return prefix_size - 1;
}