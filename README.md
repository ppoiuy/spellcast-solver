# spellcastsolver
Word Finder for Discord's SpellCast Game

spellcastsolver is a tool designed to help find valid words in Discord's SpellCast game. It provides a user-friendly interface for inputting letters, modifying tile values, and generating a list of the best words based on their value.

## How to Use

WARNING: SpellCast is bugged on the first turn of games where the 2x multiplier doesn't work despite visually showing. Don't set a 2x tile on the first turn of a game.
1. Fill in the text boxes with letters that correspond to the SpellCast game grid.
2. Use the buttons located above each text box to change a tile's value. You can set it to DL (Double Letter), TL (Triple Letter), or 2x (Double Word).
3. Adjust the slider to represent the gem number to calculate the number of swaps.
   - Note: Avoid using 9+ gems with only 3 available swaps, as it may take a few minutes to generate words. CPU intensive for 2+ swaps.
4. Click the "Solve" button to generate a list of the best words sorted by their value.
   - Note: The same word may appear multiple times in the list, but each list element has different paths or swapped tiles.
5. To view more details about a specific word, click on the word in the list on the right.
6. Toggle the checkbox in the details window of a word to display arrows in the grid, indicating the path of the word.
7. Click the reset buttons to reset the button states of each tile.

Requires 

## To-Do

- Set tool tips for each button/text box
- Use larger, more accurate word list
- Scrape 1-letter words and banned "profanity" words from the word list.
- Implement a gem tracker and net gem change in the generated word list.
   - Consider adding a toggle button for each tile under the existing buttons.
- Warning text if no DL or TL multiplier set
- Custom multiplier options for adventure mode cus look at that screenshot below lmaooooooooooooooo

---

lol how (stacking multipliers is additive e.g. D with DL + TL + TL is 3x(2+3+3))

![image](https://github.com/ppoiuy/spellcastsolver/assets/21088852/47679a26-452e-47ba-9258-b7f7bdce964a)



