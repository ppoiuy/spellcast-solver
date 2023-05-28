#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <limits>
#include <queue>
#include "solver.h"
#include "trie.h"
#include "wordlist.h"

const int SIZE = 5;  // Grid size

bool Word::operator<(const Word& other) const {
    return value > other.value;
}

// Function to check if the given coordinates are within the grid
bool isValidCell(int row, int col) {
    return (row >= 0 && row < SIZE && col >= 0 && col < SIZE);
}

// Function to calculate the value of a word based on the letter values and multipliers
int calculateWordValue(const std::vector<std::pair<int, int>>& path, const std::vector<std::vector<char>>& grid, const std::map<char, int>& letterValues, const std::vector<std::vector<int>>& letterMultipliers, int wordMultiplier) {
    int value = 0;
    for (const auto& coordinate : path) {
        int row = coordinate.first;
        int col = coordinate.second;
        char letter = grid[row][col];
        int letterValue = letterValues.at(tolower(letter));
        int letterMultiplier = letterMultipliers[row][col];
        value += letterValue * letterMultiplier;
    }
    value *= wordMultiplier;
    if (path.size() >= 6)
    {
        value += 10;
    }
    return value;
}

// Function to perform DFS traversal to find the word with the highest value
void dfs(std::vector<std::vector<char>>& grid, Trie& validWords, const Cell& currCell, const std::string& prefix, std::priority_queue<Word>& topWords,
std::vector<std::vector<bool>>& visited, const std::map<char, int>& letterValues, const std::vector<std::vector<int>>& letterMultipliers,
const std::vector<std::vector<bool>>& wordMultipliers, int wordMultiplier, int maxSwaps, std::vector<std::pair<int, int>>& path,
std::vector<std::pair<std::pair<int, int>, char>>& swappedTiles, int scoreNumber) {
    // Check if the current prefix is a valid word
    std::string prefixLowerCase = prefix;
    transform(prefixLowerCase.begin(), prefixLowerCase.end(), prefixLowerCase.begin(), ::tolower);

    // Check if any of the path tiles has a word multiplier
    bool hasWordMultiplier = false;
    for (const auto& tile : path) {
        if (wordMultipliers[tile.first][tile.second]) {
            hasWordMultiplier = true;
            break;
        }
    }
    // this code is extremely inefficient but i dont care
    int newWordMultiplier = wordMultiplier;
    if (wordMultipliers[currCell.row][currCell.col] || hasWordMultiplier) {
        newWordMultiplier = 2;
    }


    if (validWords.searchWord(prefixLowerCase)) {
        int value = calculateWordValue(path, grid, letterValues, letterMultipliers, wordMultiplier);
        if (topWords.size() < scoreNumber || value > topWords.top().value) {
            Word newWord;
            newWord.word = prefixLowerCase;
            newWord.value = value;
            newWord.path = path;
            newWord.swappedTiles = swappedTiles;
            newWord.grid = grid;
            topWords.push(newWord);
            if (topWords.size() > scoreNumber) {
                topWords.pop();
            }
        }
    }

    // Define the possible directions: up, down, left, right, diagonals
    const std::vector<int> dr = {-1, 1, 0, 0, -1, 1, -1, 1};
    const std::vector<int> dc = {0, 0, -1, 1, -1, 1, 1, -1};

    // Iterate over all possible directions
    for (int i = 0; i < 8; ++i) {
        int newRow = currCell.row + dr[i];
        int newCol = currCell.col + dc[i];

        // Check if the new cell is within the grid and hasn't been visited before
        if (isValidCell(newRow, newCol) && grid[newRow][newCol] != '\0' && !visited[newRow][newCol]) {
            char temp = grid[newRow][newCol];

            // Mark the current cell as visited
            visited[newRow][newCol] = true;

            // Check if the prefix is a prefix of any word in the set
            bool isPrefix = false;
            std::string newPrefix = prefix + temp;
            if (validWords.searchPrefix(newPrefix)) {
                isPrefix = true;
            }

            // Recursively traverse to the next cell only if the prefix is a prefix of at least one word
            if (isPrefix) {
                path.push_back({newRow, newCol});
                dfs(grid, validWords, {newRow, newCol}, newPrefix, topWords, visited, letterValues, letterMultipliers, wordMultipliers, newWordMultiplier, maxSwaps, path, swappedTiles, scoreNumber);
                path.pop_back();
            }

            // Restore the original value of the current cell
            visited[newRow][newCol] = false;
            grid[newRow][newCol] = temp;
        }
    }

    // Check if there are remaining swaps available
    if (maxSwaps > 0) {
        // Iterate over the adjacent cells and the current cell itself
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                int newRow = currCell.row + dr;
                int newCol = currCell.col + dc;

                // Check if the new cell is within the grid bounds and hasn't been visited before
                if (isValidCell(newRow, newCol) && !visited[newRow][newCol]) {
                    char temp = grid[newRow][newCol];

                    // Perform a swap and recursively traverse to the next cell only if the prefix is a prefix of at least one word
                    for (char letter = 'a'; letter <= 'z'; ++letter) {
                        std::string newPrefix = prefix + letter;

                        // Check if the new prefix is a prefix of any word in the set
                        bool isPrefix = false;
                        if (validWords.searchPrefix(newPrefix)) {
                            isPrefix = true;
                        }

                        // Perform a swap and recursively traverse to the next cell only if the prefix is a prefix of at least one word
                        if (isPrefix) {
                            visited[newRow][newCol] = true;
                            swappedTiles.push_back({{newRow, newCol}, letter});  // Store the swapped tile coordinates and the letter it changed to
                            grid[newRow][newCol] = letter;
                            path.push_back({newRow, newCol});
                            dfs(grid, validWords, {newRow, newCol}, newPrefix, topWords, visited, letterValues, letterMultipliers, wordMultipliers, wordMultiplier, maxSwaps - 1, path, swappedTiles, scoreNumber);
                            path.pop_back();
                            visited[newRow][newCol] = false;
                            grid[newRow][newCol] = temp;
                            swappedTiles.pop_back();  // Revert the swapped tile coordinates
                        }
                    }
                }
            }
        }
    }
}

void initializeValidWordTrie(Trie& validWords)
{
    std::istringstream wordStream(WORD_LIST);  // Use a string stream to read from the word list string

    std::string word;
    while (std::getline(wordStream, word)) {
        validWords.insertWord(word);
    }
}

void initializeValues(std::map<char, int> &letterValues, std::vector<std::vector<int>> &letterMultipliers, std::vector<std::vector<bool>> &wordMultipliers)
{
    //std::map<char, int>
    letterValues = {
        {'a', 1},
        {'b', 4},
        {'c', 5},
        {'d', 3},
        {'e', 1},
        {'f', 5},
        {'g', 3},
        {'h', 4},
        {'i', 1},
        {'j', 7},
        {'k', 6},
        {'l', 3},
        {'m', 4},
        {'n', 2},
        {'o', 1},
        {'p', 4},
        {'q', 8},
        {'r', 2},
        {'s', 2},
        {'t', 2},
        {'u', 4},
        {'v', 5},
        {'w', 5},
        {'x', 7},
        {'y', 4},
        {'z', 8}
    };

    // Letter multipliers
    //std::vector<std::vector<int>>
    letterMultipliers = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };

    // Word multipliers
    //std::vector<std::vector<bool>>
    wordMultipliers = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };
}

//topWords is a minheap
void solve(std::vector<Word>& topWordsVect, std::vector<std::vector<char>>& grid, int &maxSwaps, Trie& validWords, std::map<char, int> &letterValues, std::vector<std::vector<int>> &letterMultipliers, std::vector<std::vector<bool>> &wordMultipliers, int scoreNumber) {

    std::priority_queue<Word> topWords;
    std::vector<std::vector<bool>> visited(SIZE, std::vector<bool>(SIZE, false));
    std::vector<std::pair<int, int>> path;  // Stores the path through the grid
    std::vector<std::pair<std::pair<int, int>, char>> swappedTiles;  // Stores the swapped tiles

    //std::cout << "Solving...\n";

    // Perform DFS traversal starting from each cell in the grid
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            Cell startCell = {row, col};
            visited[row][col] = true;
            path.push_back({row, col});
            dfs(grid, validWords, startCell, std::string(1, grid[row][col]), topWords, visited, letterValues, letterMultipliers, wordMultipliers, 1, maxSwaps, path, swappedTiles, scoreNumber);
            visited[row][col] = false;
            path.pop_back();

            // Check if there are remaining swaps available for the start cell
            if (maxSwaps > 0) {
                char temp = grid[row][col];
                // Perform a swap and recursively traverse to the next cell only if the prefix is a prefix of at least one word
                for (char letter = 'a'; letter <= 'z'; ++letter) {
                    std::string newPrefix = std::string(1, letter);
                    // Check if the new prefix is a prefix of any word in the set
                    bool isPrefix = false;
                    if (validWords.searchPrefix(newPrefix)) {
                        isPrefix = true;
                    }
                    // Perform a swap and recursively traverse to the next cell only if the prefix is a prefix of at least one word
                    if (isPrefix) {
                        visited[row][col] = true;
                        swappedTiles.push_back({{row, col}, letter});  // Store the swapped tile coordinates and the letter it changed to
                        grid[row][col] = letter;
                        path.push_back({row, col});
                        dfs(grid, validWords, {row, col}, newPrefix, topWords, visited, letterValues, letterMultipliers, wordMultipliers, 1, maxSwaps - 1, path, swappedTiles, scoreNumber);
                        path.pop_back();
                        visited[row][col] = false;
                        grid[row][col] = temp;
                        swappedTiles.pop_back();  // Revert the swapped tile coordinates
                    }
                }
            }
        }
    }

    // Copying the top scores from the priority queue to the vector
    while (!topWords.empty()) {
        topWordsVect.push_back(topWords.top());
        topWords.pop();
    }

    // Sorting the scores in descending order
    reverse(topWordsVect.begin(), topWordsVect.end());
}
