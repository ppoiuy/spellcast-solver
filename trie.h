#ifndef TRIE_H
#define TRIE_H

#include <unordered_map>
#include <string>

class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    bool isWord;

    TrieNode();
};

class Trie {
private:
    TrieNode* root;

public:
    Trie();
    ~Trie();

    void destroyNode(TrieNode* node);
    void insertWord(const std::string& word);
    bool searchWord(const std::string& word);
    bool searchPrefix(const std::string& prefix);
};

#endif 