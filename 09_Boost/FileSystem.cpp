#include "FileSystem.h"

#include <algorithm>
#include <iostream>

std::unordered_map<size_t, std::list<std::filesystem::path>> FileSystem::ListFiles(bool recursively = true) const {
    std::unordered_map<size_t, std::list<std::filesystem::path>> list;

    for (const auto& dir : m_directories) {
        GetDirectoryFiles(dir, list, recursively);
    }

    return list;
}

void FileSystem::GetDirectoryFiles(const std::filesystem::path& dir,
                                   std::unordered_map<size_t, std::list<std::filesystem::path>>& list,
                                   bool recursively = true) const {
    for (auto const& dir_entry : std::filesystem::directory_iterator{dir}) {
        if (!std::filesystem::is_directory(dir_entry.path())) {
            if (dir_entry.file_size() >= m_min_file_size) {
                std::smatch base_match;
                if (m_has_mask && std::regex_match(dir_entry.path().filename().string(), m_txt_regex)) {
                    list[dir_entry.file_size()].push_back(dir_entry.path());
                }
            }
        } else if (recursively) {
            if (std::find(m_directories_exclusion.begin(), m_directories_exclusion.end(), dir_entry.path()) ==
                m_directories_exclusion.end()) {
                GetDirectoryFiles(dir_entry.path(), list, recursively);
            }
        }
    }
}