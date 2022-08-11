#pragma once

#include <iostream>
#include <tuple>

#include "mapreduce.h"

size_t calc_min_prefix(std::istream& is, const std::filesystem::path& output) {
    int mappers_count = 1;
    int reducers_count = 1;

    MapReduce mr(mappers_count, reducers_count);

    size_t prefix_size = 1;

    bool run = true;

    while (run) {
        mr.set_mapper([prefix_size](const std::string& line) -> auto{
            // моё предложение:
            //     * получает строку,
            //     * выделяет префикс,
            //     * возвращает пары (префикс, 1).
            return std::tuple{line.substr(0, prefix_size), 1, line.size()};
        });

        mr.set_reducer([](auto res, auto pair) -> auto{
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
            if (std::get<1>(res) > 1)
                return res;
            else if (std::get<0>(res) == std::get<0>(pair)) {
                ++std::get<1>(res);
                return res;
            }
            return pair;
        });

        mr.run(is, output);

        for (auto const& dir_entry : std::filesystem::directory_iterator{output}) {
            if (!std::filesystem::is_directory(dir_entry.path())) {
                std::ifstream fin(dir_entry.path());
                size_t num_duplicates, max_len;
                fin >> num_duplicates >> max_len;
                if (num_duplicates == 1 || max_len <= prefix_size) {
                    run = false;
                    break;
                }
            }
        }

        ++prefix_size;

        is.clear();
        is.seekg(0, std::ios_base::beg);
    }

    return prefix_size - 1;
}