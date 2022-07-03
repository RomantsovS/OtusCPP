#pragma once

#include <filesystem>
#include <list>

class HashTree {
   public:
    struct HashNode {
        HashNode() = default;
        explicit HashNode(size_t key) : m_key(key) {}

        size_t m_key;
        std::list<std::filesystem::path> m_value;

        std::unordered_map<size_t, HashNode*> m_children;
    };

    HashTree::HashNode* Add(size_t key, std::filesystem::path value, HashTree::HashNode* node = nullptr);
    void Remove(HashNode* node, const std::filesystem::path& value);
    void Print() const;

    template <typename F>
    void TraverseTree(F f);

   private:
    std::pair<HashNode*, bool> Find(HashNode* node, size_t key);
    void PrintNode(HashTree::HashNode* node, size_t offset = 0) const;

    template <typename F>
    void TraverseNode(HashNode* node, F f);

    HashNode* root = nullptr;
};

template <typename F>
void HashTree::TraverseTree(F f) {
    TraverseNode(root, f);
}

template <typename F>
void HashTree::TraverseNode(HashNode* node, F f) {
    if (!node) return;

    f(node);

    for (auto& ch : node->m_children) {
        TraverseNode(ch.second, f);
    }
}