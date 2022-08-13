/**
 * Это MapReduce фреймворк.
 * Он универсальный.
 * Он может выполнять разные map_reduce задачи.
 * Он просто обрабатывает какие-то данные с помощью каких-то функций в нескольких потоках.
 * Он ничего не знает о задаче, которую решает.
 * Здесь не должно быть кода, завязанного на конкретную задачу - определение длины префикса.
 *
 * С помощью этого фреймворка должны решаться разные задачи.
 * Когда напишете это, попробуйте решить с помощью этого фреймворка все задачи, которые мы разбирали
 * на лекции.
 *
 * Это наш самописный аналог hadoop mapreduce.
 * Он на самом деле не работает с по-настоящему большими данными, потому что выполняется на одной
 * машине. Но мы делаем вид, что данных много, и представляем, что наши потоки - это процессы на
 * разных узлах.
 *
 * Ни один из потоков не должен полностью загружать свои данные в память или пробегаться по всем
 * данным. Каждый из потоков обрабатывает только свой блок.
 *
 * На самом деле даже один блок данных не должен полностью грузиться в оперативку, а должен
 * обрабатываться построчно. Но в домашней работе можем этим пренебречь и загрузить один блок в
 * память одним потоком.
 *
 * Всё в этом файле - это рекомендация.
 * Если что-то будет слишком сложно реализовать, идите на компромисс, пренебрегайте чем-нибудь.
 * Лучше сделать что-нибудь, чем застрять на каком-нибудь моменте и не сделать ничего.
 */

#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <string>
#include <thread>
#include <vector>

template <typename T>
class MapReduce {
   public:
    MapReduce(size_t mappers_count, size_t reducers_count)
        : m_mappers_count(mappers_count), m_reducers_count(reducers_count) {}

    using mapper_type = std::function<T(const std::string&)>;
    void set_mapper(mapper_type mapper) { m_mapper = mapper; }

    using reducer_type = std::function<T(T, T)>;
    void set_reducer(reducer_type reducer) { m_reducer = reducer; }

    void run(std::istream& input, const std::filesystem::path& output) {
        std::filesystem::remove_all(output);
        std::filesystem::create_directory(output);

        std::filesystem::path map_out = output / "map";
        std::filesystem::create_directory(map_out);

        std::filesystem::path reduce_out = output / "reduce";
        std::filesystem::create_directory(reduce_out);

        map(input, map_out);

        distribute(map_out, reduce_out);

        reduce(reduce_out, output);

        // Создаём mappers_count потоков
        // В каждом потоке читаем свой блок данных
        // Применяем к строкам данных функцию mapper
        // Сортируем результат каждого потока
        // Результат сохраняется в файловую систему (представляем, что это большие данные)
        // Каждый поток сохраняет результат в свой файл (представляем, что потоки выполняются на
        // разных узлах)

        // Создаём reducers_count новых файлов
        // Из mappers_count файлов читаем данные (результат фазы map) и перекладываем в
        // reducers_count (вход фазы reduce) Перекладываем так, чтобы:
        //     * данные были отсортированы
        //     * одинаковые ключи оказывались в одном файле, чтобы одинаковые ключи попали на один
        //     редьюсер
        //     * файлы примерно одинакового размера, чтобы редьюсеры были загружены примерно
        //     равномерно
        //
        // Гуглить: алгоритмы во внешней памяти, external sorting, многопутевое слияние
        //
        // Для упрощения задачи делаем это в один поток
        // Но все данные в память одновременно не загружаем, читаем построчно и пишем
        //
        // Задание творческое!
        // Я не уверен, что все вышеперечисленные требования выполнимы одновременно
        // Возможно, придётся идти на компромисс, упрощая какие-то детали реализации
        // Но это то, к чему нужно стремиться
        // Проектирование ПО часто требует идти на компромиссы
        // Это как оптимизация функции многих переменных с доп. ограничениями

        // Создаём reducers_count потоков
        // В каждом потоке читаем свой файл (выход предыдущей фазы)
        // Применяем к строкам функцию reducer
        // Результат сохраняется в файловую систему
        //             (во многих задачах выход редьюсера - большие данные, хотя в нашей задаче
        //             можно написать функцию reduce так, чтобы выход не был большим)
    }

   private:
    struct Block {
        size_t from;
        size_t to;
    };

    std::vector<Block> split_file(std::istream& is, size_t blocks_count) {
        /**
         * Эта функция не читает весь файл.
         *
         * Определяем размер файла в байтах.
         * Делим размер на количество блоков - получаем границы блоков.
         * Читаем данные только вблизи границ.
         * Выравниваем границы блоков по границам строк.
         */

        std::vector<Block> res(blocks_count);

        is.seekg(0, std::ios::end);
        auto fs = is.tellg();
        auto block_size = fs / blocks_count;

        std::vector<char> buffer(10);

        size_t pos = 0;

        for (size_t i = 0; i < blocks_count; ++i) {
            res[i].from = pos;

            pos = block_size * (i + 1) - 1;

            is.seekg(pos, std::ios_base::beg);

            while (true) {
                is.read(buffer.data(), 1);
                if (!is.gcount() || buffer[0] == '\n') break;
                ++pos;
            }

            res[i].to = pos++;
        }

        is.clear();
        is.seekg(0, std::ios_base::beg);

        return res;
    }

    void map(std::istream& input, const std::filesystem::path& map_out) {
        auto blocks = split_file(input, m_mappers_count);

        std::vector<std::future<void>> futures;

        auto f = [this, &input, &blocks, &map_out](size_t i) {
            std::vector<std::string> svec;

            std::string line;
            while (std::getline(input, line)) {
                svec.push_back(line);
                auto p = input.tellg();
                if (p < 0 || static_cast<size_t>(p) > blocks[i].to) break;
            }

            std::vector<T> map_output;
            std::transform(svec.begin(), svec.end(), std::back_inserter(map_output), m_mapper);
            sort(map_output.begin(), map_output.end());

            std::ofstream fout(map_out / (std::to_string(i) + get_file_name()));
            for (const auto& p : map_output) {
                fout << p << "\n";
            }
        };

        for (size_t i = 0; i < blocks.size(); ++i) {
            futures.push_back(std::async(std::launch::async, f, i));
        }

        for (auto& task : futures) task.get();
    }

    void distribute(const std::filesystem::path& map_out, const std::filesystem::path& reduce_out) {
        std::vector<std::ofstream> out_vec;
        for (size_t i = 0; i < m_reducers_count; ++i) {
            auto out_path = reduce_out / (std::to_string(i) + get_file_name());
            out_vec.emplace_back(out_path);
        }

        std::priority_queue<std::pair<T, size_t>, std::vector<std::pair<T, size_t>>,
                            std::greater<std::pair<T, size_t>>>
            q;
        std::vector<std::ifstream> vec;

        for (auto const& dir_entry : std::filesystem::directory_iterator{map_out}) {
            if (!std::filesystem::is_directory(dir_entry.path())) {
                vec.emplace_back(dir_entry.path());
            }
        }

        T data;

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] >> data) q.push({data, i});
        }

        T last_data = q.top().first;
        size_t index = 0;

        while (!q.empty()) {
            auto p = q.top();
            q.pop();

            if (last_data != p.first) {
                last_data = p.first;
                index = (index + 1) % m_reducers_count;
            }

            out_vec[index] << p.first << '\n';

            if (vec[p.second] >> data) q.push({data, p.second});
        }
    }

    void reduce(const std::filesystem::path& reduce_out, const std::filesystem::path& output) {
        auto f = [this, &output](const std::filesystem::path& path) {
            std::vector<T> map_output;

            std::ifstream fin(path);

            T data;
            while (fin >> data) {
                map_output.push_back(data);
            }

            if(map_output.empty())
                return;

            const auto last_word =
                std::accumulate(map_output.begin(), map_output.end(), T(), m_reducer);

            std::ofstream fout(output / get_file_name());
            fout << last_word << '\n';
        };

        std::vector<std::future<void>> futures;

        for (auto const& dir_entry : std::filesystem::directory_iterator{reduce_out}) {
            if (!std::filesystem::is_directory(dir_entry.path())) {
                futures.push_back(std::async(std::launch::async, f, dir_entry.path()));
            }
        }

        for (auto& task : futures) task.get();
    }

    std::string get_file_name() {
        auto t = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(t);

        std::ostringstream oss;
        oss << std::this_thread::get_id() << "_" << tt;

        return oss.str();
    }

    size_t m_mappers_count;
    size_t m_reducers_count;

    mapper_type m_mapper;
    reducer_type m_reducer;
};