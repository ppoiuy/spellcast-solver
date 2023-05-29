# spellcast-solver
Word finder for Discord's SpellCast game

Allows for inputting letters, modifying tile values, and generating a list of the best words based on their value, showing each path and which letters to swap.

## How to Use

WARNING: SpellCast is bugged on the first turn of games where the 2x multiplier doesn't work despite visually showing. Don't set a 2x tile on the first turn of a game.
1. Fill in the text boxes with letters that correspond to the SpellCast game grid.
2. Use the buttons located above each text box to change a tile's value. You can set it to DL (Double Letter), TL (Triple Letter), and/or 2x (Double Word).
3. Adjust the slider to represent the gem number to calculate the number of swaps.
   - Note: Avoid using 9+ gems with only 3 available swaps, as it may take a few minutes to generate words. CPU intensive for 2+ swaps.
4. Click the rectangle buttons under the corresponding text box to represent that tile having a gem.
5. Click the "Solve" button to generate a list of the best words sorted by their value.
   - Note: The same word may appear multiple times in the list, but each list element has different paths or swapped tiles.
6. To view more details about a specific word, click on the word in the list on the right.
   - Green: Path to take
   - Dark Green: First tile in the path
   - Red Outline: Swapped tiles
7. Click the reset buttons to reset the button states of every tile.

![image](https://github.com/ppoiuy/spellcastsolver/assets/21088852/75cde797-0a14-4926-8120-f16567b2a20c)

![image](https://github.com/ppoiuy/spellcastsolver/assets/21088852/339d06d7-24c4-4ea5-b544-141b61233f42)

![image](https://github.com/ppoiuy/spellcastsolver/assets/21088852/d5c35592-5a56-4529-ad12-e093fc309675)

## To-Do
- Sort by net gems after sorting by prio queue? need to add net gem condition in overloaded operator if equal
- Set tool tips for each button/text box
   - Tool tip of swapped tiles can use the original grid's value to show what the previous letter was before being swapped.
   - Most button tooltips are just use cases: limit multiplier checkbox use case is only adventure mode
- Pasting string of text should overflow to next text boxes, or import line of text option
- Use larger, more accurate word list
- Custom multiplier options for adventure mode
---

(stacking multipliers is additive)

![image](https://github.com/ppoiuy/spellcastsolver/assets/21088852/47679a26-452e-47ba-9258-b7f7bdce964a)
