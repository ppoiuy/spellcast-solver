#include <unordered_map>
#include <string>
#include "trie.h"

TrieNode::TrieNode() {
    isWord = false;
}

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    destroyNode(root);
}

void Trie::destroyNode(TrieNode* node) {
    if (node == nullptr) {
        return;
    }

    for (auto& pair : node->children) {
        destroyNode(pair.second);
    }

    delete node;
}

void Trie::insertWord(const std::string& word) {
    TrieNode* current = root;

    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = new TrieNode();
        }
        current = current->children[c];
    }

    current->isWord = true;
}

bool Trie::searchWord(const std::string& word) {
    TrieNode* current = root;

    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            return false;
        }
        current = current->children[c];
    }

    return current->isWord;
}

bool Trie::searchPrefix(const std::string& prefix) {
    TrieNode* current = root;

    for (char c : prefix) {
        if (current->children.find(c) == current->children.end()) {
            return false;
        }
        current = current->children[c];
    }
    return true;
}