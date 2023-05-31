# spellcast-solver
Word finder for Discord's SpellCast game

Allows for inputting letters, modifying tile values, and generating a list of the best words based on their value, showing each path and which letters to swap.

## Usage

Note: SpellCast is bugged on the first turn of games where the 2x multiplier doesn't work despite visually showing. Don't set a 2x tile on the first turn of a game.
- Fill in the text boxes with letters that correspond to the SpellCast game grid.
- Use the buttons located above each text box to change a tile's value. You can set it to DL (Double Letter), TL (Triple Letter), and/or 2x (Double Word).
- Adjust the slider to represent the gem number to calculate the number of swaps.
   - Note: Avoid using 9+ gems with only 3 available swaps, as it may take a few minutes to generate words. CPU intensive for 2+ swaps.
- Click the rectangle buttons under the corresponding text box to represent that tile having a gem.
- Click the "Solve" button to generate a list of the best words sorted by their value (word - value (net gems)).
   - Note: The same word may appear multiple times in the list, but each list element has different paths or swapped tiles.
- To view more details about a specific word, click on the word in the list on the right.
   - Green: Path to take
   - Dark Green: First tile in the path
   - Red Outline: Swapped tiles
- Click the reset buttons to reset the corresponding button states of every tile.

![image](https://github.com/ppoiuy/spellcastsolver/assets/21088852/75cde797-0a14-4926-8120-f16567b2a20c)

![image](https://github.com/ppoiuy/spellcast-solver/assets/21088852/4eb7abb1-2f26-49b8-84b2-ef1a3544bdf7)

![image](https://github.com/ppoiuy/spellcast-solver/assets/21088852/b9b87e2d-6914-4fd3-b407-9ab113711550)

## To-Do
- Tool tips
- Pasting string of text should overflow to next text boxes, or add import line of text option
- Use larger, more accurate word list
- Custom multiplier options for adventure mode
- 
---

![image](https://github.com/ppoiuy/spellcastsolver/assets/21088852/47679a26-452e-47ba-9258-b7f7bdce964a)
