#include <unordered_map>
#include <string>
#include "trie.h"

TrieNode::TrieNode() {
    isWord = false;
}

Trie::Trie() {
    root = new TrieNode();
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