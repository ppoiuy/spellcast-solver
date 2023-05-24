// accounts for 0-3 swaps, but 3 takes too long
// prints top 3 scores, determined by const int SCORE_NUMBER
// even if the word is the same, it will account for different paths
// 
// make gui later
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <limits>
#include <queue>

const int SIZE = 5;  // Grid size
const int SCORE_NUMBER = 3; // number of top scores saved
const std::string VALID_WORDS_FILENAME = "valid_words.txt";
const std::string INPUT_FILENAME = "input.txt";

// Structure to store coordinates of a cell
struct Cell {
    int row;
    int col;
};

class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    bool isWord;

    TrieNode() {
        isWord = false;
    }
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    void insertWord(const std::string& word) {
        TrieNode* current = root;

        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];
        }

        current->isWord = true;
    }

    bool searchWord(const std::string& word) {
        TrieNode* current = root;

        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                return false;
            }
            current = current->children[c];
        }

        return current->isWord;
    }

    bool searchPrefix(const std::string& prefix) {
        TrieNode* current = root;

        for (char c : prefix) {
            if (current->children.find(c) == current->children.end()) {
                return false;
            }
            current = current->children[c];
        }
        return true;
    }

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

    bool operator<(const Word& other) const {
        return value > other.value;
    }
};

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
std::vector<std::pair<std::pair<int, int>, char>>& swappedTiles) {
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
        if (topWords.size() < 5 || value > topWords.top().value) {
            Word newWord;
            newWord.word = prefixLowerCase;
            newWord.value = value;
            newWord.path = path;
            newWord.swappedTiles = swappedTiles;
            newWord.grid = grid;
            topWords.push(newWord);
            if (topWords.size() > 5) {
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
                dfs(grid, validWords, {newRow, newCol}, newPrefix, topWords, visited, letterValues, letterMultipliers, wordMultipliers, newWordMultiplier, maxSwaps, path, swappedTiles);
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
                            dfs(grid, validWords, {newRow, newCol}, newPrefix, topWords, visited, letterValues, letterMultipliers, wordMultipliers, wordMultiplier, maxSwaps - 1, path, swappedTiles);
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

int main() {
    Trie validWords;

    std::ifstream wordFile(VALID_WORDS_FILENAME);
    if (!wordFile) {
        std::cout << "Failed to open the word file." << std::endl;
        return 1;
    }
    
    std::string word;
    while (wordFile >> word) {
        validWords.insertWord(word);
    }
    wordFile.close();
    
    // Letter values
    std::map<char, int> letterValues = {
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
    std::vector<std::vector<int>> letterMultipliers = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };
    
    // Word multipliers
    std::vector<std::vector<bool>> wordMultipliers = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };

    bool hasValidInputFile = false;
    std::ifstream inputFile(INPUT_FILENAME);

    if (!inputFile) {
        std::ofstream createFile(INPUT_FILENAME);
        if (!createFile) {
            std::cerr << "Failed to create file." << std::endl;
            return 1;
        }
        createFile.close();
        inputFile.open(INPUT_FILENAME);
    }

    // Check if the file is empty
    inputFile.seekg(0, std::ios::end);
    if (inputFile.tellg() != 0) {
        hasValidInputFile = true;
    }
    inputFile.close();

    char input, prev_input = '\0';
    std::string message;
    std::vector<std::vector<char>> grid(SIZE, std::vector<char>(SIZE));

    if (!hasValidInputFile) {
        std::cout << "Enter the letters for the 5x5 grid:" << std::endl;
        std::cout << "Lowercase is regular input" << std::endl;
        std::cout << "Uppercase is 2X double word input" << std::endl;
        std::cout << "Put number (2 or 3) after a letter if DL/TL" << std::endl;
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (!hasValidInputFile)
                {
                    std::cin >> input;
                    if (isalpha(input)) // letters
                    {
                        grid[row][col] = tolower(input); // uppercase = double word
                        if (isupper(input))
                        {
                            wordMultipliers[row][col] = 1; // true
                            message = message + (char)tolower(input) + " at (" + std::to_string(row) + "," + std::to_string(col) + ") set as double word\n";
                        }

                    } 
                    else if ((input == '2' || input == '3') && prev_input != '\0') // double letter or triple letter
                    {   
                        if (col == 0) {
                            --row;
                            col = (SIZE-1);  // Go back one iteration of outer loop
                        } else {
                            --col;  // Go back one iteration of inner loop
                        }
                        letterMultipliers[row][col] = input - '0'; // convert to int

                        message = message + prev_input + " at (" + std::to_string(row) + "," + std::to_string(col) + ") set with x" + input + " value\n";
                    }
                    else if (prev_input == '\0') // trying to set no value as DL or TL
                    {
                        std::cout << "error, no previous value to set as DL/TL";
                        return 1;
                    }
                    else // invalid input
                    {
                        std::cout << "invalid input\n";
                        return 1;
                    }
                    prev_input = input;
                }
            }
        }
        std::cout << message;
    }
    else
    {
        std::cout << "Using " << INPUT_FILENAME << " for input" << std::endl;
        std::cout << "(If input file is incomplete, last letter will repeat)" << std::endl;
        std::ifstream inputFile(INPUT_FILENAME);
        inputFile.seekg(0, std::ios::beg);
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (!hasValidInputFile)
                {
                    inputFile >> input;
                }
                else
                {
                    inputFile >> input;
                }
                if (isalpha(input)) // letters
                {
                    grid[row][col] = tolower(input); // uppercase = double word
                    if (isupper(input))
                    {
                        wordMultipliers[row][col] = 1; // true
                        message = message + (char)tolower(input) + " at (" + std::to_string(row) + "," + std::to_string(col) + ") set as double word\n";
                    }

                } 
                else if ((input == '2' || input == '3') && prev_input != '\0') // double letter or triple letter
                {   
                    if (col == 0) {
                        --row;
                        col = (SIZE-1);  // Go back one iteration of outer loop
                    } else {
                        --col;  // Go back one iteration of inner loop
                    }
                    letterMultipliers[row][col] = input - '0'; // convert to int

                    message = message + prev_input + " at (" + std::to_string(row) + "," + std::to_string(col) + ") set with x" + input + " value\n";
                }
                else if (prev_input == '\0') // trying to set no value as DL or TL
                {
                    std::cout << "error, no previous value to set as DL/TL";
                    return 1;
                }
                else // invalid input
                {
                    std::cout << "invalid input\n";
                    return 1;
                }
                prev_input = input;
            }
        }
        std::cout << message;
    }
    

    // Iterate through the letterMultipliers matrix
    int count = 0;
    for (const auto& row : letterMultipliers) {
        for (int num : row) {
            if (num > 1) {
                count++;
                if (count > 1) { // Found more than one number greater than 1
                    std::cout << "error: setting multiple values to DL/TL\n";
                    return 1;  
                }
            }
        }
    }

    // Iterate through the wordMultipliers matrix
    int trueCount = 0;
    for (const auto& row : wordMultipliers) {
        for (const bool element : row) {
            if (element) {
                trueCount++;
                if (trueCount > 1) { // more than 1 true value is found
                    std::cout << "error: setting multiple values to double word\n";
                    return 1;
                }
            }
        }
    }

    // Clear the input buffer if user input taken beforehand
    if (!hasValidInputFile)
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    // ask for max swaps ((int)gems/3)
    int maxSwaps;
    std::cout << "Enter the maximum number of swaps (0-3, but 3 takes way too long): ";
        while (!(std::cin >> maxSwaps) || (maxSwaps < 0 || maxSwaps > 3)) {
        std::cout << "Invalid input. Please enter a valid integer from 0 to 3: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::priority_queue<Word> topWords; // Min-heap
    std::vector<Word> topWordsVect; // Stores top words in descending order

    std::vector<std::vector<bool>> visited(SIZE, std::vector<bool>(SIZE, false));
    std::vector<std::pair<int, int>> path;  // Stores the path through the grid
    std::vector<std::pair<std::pair<int, int>, char>> swappedTiles;  // Stores the swapped tiles
    
    std::cout << "Solving...\n";

    int percent = 0;
    // Perform DFS traversal starting from each cell in the grid
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            Cell startCell = {row, col};
            visited[row][col] = true;
            path.push_back({row, col});
            dfs(grid, validWords, startCell, std::string(1, grid[row][col]), topWords, visited, letterValues, letterMultipliers, wordMultipliers, 1, maxSwaps, path, swappedTiles);
            visited[row][col] = false;
            path.pop_back(); 
        }
        percent += 20;
        std::cout << percent << "%" << std::endl;
    }
    std::cout << std::endl;

    // Copying the top scores from the priority queue to the vector
    while (!topWords.empty()) {
        topWordsVect.push_back(topWords.top());
        topWords.pop();
    }

    // Sorting the scores in descending order
    reverse(topWordsVect.begin(), topWordsVect.end());

    // Printing the top x scores, determined by SCORE_NUMBER
    if (topWordsVect.empty()) {
        std::cout << "No valid word found." << std::endl;
    } else {
        for (int i = 0; i < SCORE_NUMBER && i < (int)topWordsVect.size(); ++i) {
            const Word& word = topWordsVect[i];
            std::cout << i+1 << ": " << std::endl;
            std::cout << "Word: " << word.word << ", Value: " << word.value << std::endl;
            std::cout << "Path:";
            for (const auto& coordinate : word.path) {
                std::cout << "(" << coordinate.first << ", " << coordinate.second << ") ";
            }
            std::cout << std::endl;
            std::cout << "Swapped Tiles:" << std::endl;
            for (const auto& entry : word.swappedTiles) {
                std::cout << "(" << entry.first.first << ", " << entry.first.second << ", " << entry.second << ") ";
            }
            std::cout << std::endl;
            std::cout << "Grid:" << std::endl;
            for (const auto& row : word.grid) {
                for (const auto& letter : row) {
                    std::cout << letter << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    return 0;
}
