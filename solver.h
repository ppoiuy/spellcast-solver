#ifndef SOLVER_H
#define SOLVER_H
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <limits>
#include <queue>
#include "trie.h"

// Structure to store coordinates of a cell
struct Cell {
    int row;
    int col;
};

struct Word {
    std::string word;
    int value;
    std::vector<std::pair<int, int>> path;
    std::vector<std::pair<std::pair<int, int>, char>> swappedTiles;
    std::vector<std::vector<char>> grid;

    Word() : value(0) {}

    Word(const std::string& word, int value, const std::vector<std::pair<int, int>>& path, const std::vector<std::pair<std::pair<int, int>, char>>& swappedTiles, const std::vector<std::vector<char>>& grid)
        : word(word), value(value), path(path), swappedTiles(swappedTiles), grid(grid) {}

    bool operator<(const Word& other) const;
};

// Function to check if the given coordinates are within the grid
bool isValidCell(int row, int col);

// Function to calculate the value of a word based on the letter values and multipliers
int calculateWordValue(const std::vector<std::pair<int, int>>& path, const std::vector<std::vector<char>>& grid, 
const std::map<char, int>& letterValues, const std::vector<std::vector<int>>& letterMultipliers, int wordMultiplier);

// Function to perform DFS traversal to find the word with the highest value
void dfs(std::vector<std::vector<char>>& grid, Trie& validWords, const Cell& currCell, const std::string& prefix, std::priority_queue<Word>& topWords, 
std::vector<std::vector<bool>>& visited, const std::map<char, int>& letterValues, const std::vector<std::vector<int>>& letterMultipliers, 
const std::vector<std::vector<bool>>& wordMultipliers, int wordMultiplier, int maxSwaps, std::vector<std::pair<int, int>>& path, 
std::vector<std::pair<std::pair<int, int>, char>>& swappedTiles, int scoreNumber);

void initializeValidWordTrie(Trie& validWords);

void initializeValues(std::map<char, int> &letterValues, std::vector<std::vector<int>> &letterMultipliers, std::vector<std::vector<bool>> &wordMultipliers);

void solve(std::vector<Word>& topWordsVect, std::vector<std::vector<char>>& grid, int& maxSwaps, Trie& validWords, std::map<char, int> &letterValues, std::vector<std::vector<int>> &letterMultipliers, std::vector<std::vector<bool>> &wordMultipliers, int scoreNumber);

#endif  // 
