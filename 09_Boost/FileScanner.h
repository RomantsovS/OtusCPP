#pragma once

#include <map>
#include <unordered_map>
#include <list>

#include "FileSystem.h"
#include "HashTree.h"

class FileScanner {
   public:
    FileScanner(std::unordered_map<size_t, std::list<std::filesystem::path>>&& files, size_t block_size)
        : m_files(files), m_block_size(block_size) {}
    void Scan();

   private:
    bool ReadBlock(const std::list<std::filesystem::path>& list, size_t block_num);

    HashTree hash_tree;
    std::map<std::filesystem::path, HashTree::HashNode*> m_path_to_node;
    std::unordered_map<size_t, std::list<std::filesystem::path>> m_files;
    size_t m_block_size = 0;
};