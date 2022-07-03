#pragma once

#include <filesystem>
#include <list>
#include <regex>
#include <string>
#include <unordered_map>

class FileSystem {
   public:
    FileSystem(const std::vector<std::filesystem::path>& directories, const std::string& mask,
    const std::vector<std::filesystem::path>& directories_exclusion, size_t min_file_size)
        : m_directories(directories), m_txt_regex(mask), m_has_mask(!mask.empty()), 
        m_directories_exclusion(directories_exclusion), m_min_file_size(min_file_size) {}

    std::unordered_map<size_t, std::list<std::filesystem::path>> ListFiles(bool recursively) const;

   private:
    void GetDirectoryFiles(const std::filesystem::path& dir,
                           std::unordered_map<size_t, std::list<std::filesystem::path>>& list, bool recursively) const;
    std::vector<std::filesystem::path> m_directories;
    const std::regex m_txt_regex;
    bool m_has_mask = false;
    std::vector<std::filesystem::path> m_directories_exclusion;
    size_t m_min_file_size = 0;
};