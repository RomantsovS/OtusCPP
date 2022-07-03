#include "FileScanner.h"

#include <algorithm>
#include <fstream>
#include <iostream>

void FileScanner::Scan() {
    if (m_files.empty()) {
        return;
    }

    for (auto it = m_files.begin(); it != m_files.end();) {
        if (it->second.size() == 1)
            it = m_files.erase(it);
        else
            ++it;
    }

    for (auto it = m_files.begin(); it != m_files.end(); ++it) {
        size_t block_num = 0;
        bool file_read = true;

        while (file_read) {
            file_read = ReadBlock(it->second, block_num);

            ++block_num;
        }
    }

    hash_tree.TraverseTree([](const HashTree::HashNode* node) {
        if(node->m_value.size() < 2)
            return;

        for (auto val : node->m_value) {
            std::cout << val.u8string() << std::endl;
        }
        std::cout << std::endl;
    });
}

bool FileScanner::ReadBlock(const std::list<std::filesystem::path>& list, size_t block_num) {
    std::vector<char> buffer(m_block_size);
    std::hash<std::string> h;

    bool file_read = false;

    for (const auto& path : list) {
        std::fill(buffer.begin(), buffer.end(), 0);

        // std::cout << path << std::endl;

        std::ifstream fin(path);

        fin.seekg(block_num * m_block_size, std::ios_base::beg);

        fin.read(buffer.data(), m_block_size);
        if (!fin.gcount()) continue;

        // std::cout << buffer.data();

        auto hash = h(std::string(buffer.data()));

        if (m_path_to_node.count(path) == 0) {
            m_path_to_node[path] = hash_tree.Add(hash, path);
        } else {
            auto new_node = hash_tree.Add(hash, path, m_path_to_node[path]);
            hash_tree.Remove(m_path_to_node[path], path);
            m_path_to_node[path] = new_node;
        }

        // std::cout << std::endl;

        if (!fin.eof()) file_read = true;

        // hash_tree.Print();
    }

    return file_read;
}