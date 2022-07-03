#include "HashTree.h"
#include <iostream>

HashTree::HashNode* HashTree::Add(size_t key, std::filesystem::path path, HashTree::HashNode* node) {
    if (!node) {
        if (!root) {
            root = new HashNode(key);
            root->m_value.push_back(path);
            return root;
        } else {
            node = root;
        }
    }

    auto res = Find(node, key);
    if (res.second) {
        res.first->m_value.push_back(path);
        return res.first;
    } else {
        auto new_node = new HashNode(key);
        res.first->m_children[key] = new_node;
        new_node->m_value.push_back(path);
        return new_node;
    }
}

std::pair<HashTree::HashNode*, bool> HashTree::Find(HashNode* node, size_t key) {
    if (!node) {
        return {nullptr, false};
    } else if (node->m_key == key) {
        return {node, true};
    }

    if (node->m_children.count(key) != 0) {
        return Find(node->m_children[key], key);
    }

    return {node, false};
}

void HashTree::Remove(HashNode* node, const std::filesystem::path& value) { node->m_value.remove(value); }

void HashTree::Print() const {
    if (!root) {
        std::cout << "Empty tree\n";
        return;
    }

    PrintNode(root);
}

void HashTree::PrintNode(HashTree::HashNode* node, size_t offset) const {
    std::cout << std::string(offset, ' ') << "[" << node->m_key << "]=(";

    bool first = true;
    for (auto val : node->m_value) {
        if (!first) std::cout << ", ";
        std::cout << val.u8string();
        first = false;
    }

    std::cout << ")\n";

    for (auto ch : node->m_children) {
        PrintNode(ch.second, offset == 0 ? 4 : offset * 2);
    }
}