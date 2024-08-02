
Coded a chess engine with minimax alpha beta pruning. First time trying this type of project. Coding chess with all of the special moves and intricacies was difficult enough, yet I still wanted to make an engine.
- The engine works amazing compared to what I expected. There are still bugs when the depth is raised above 2, although 2 is still pretty strong.
- All special moves like En Passant, castling, promotion, and things like pins and discovery checks are all included.
- I tested the engine against a 1000 elo bot on chess dot com and my engine won overwhelmingly by checkmate (80% accuracy).
- The primary flaw that exists currently is that the bot will infinitely repeat moves.

The code is messy although I didn't want to focus on writing it perfectly and cleanly as this is my first time and I just wanted an introduction into the space.

If you would like to compile and play against the engine: `g++ main.cpp engine.cpp && ./a.out`
- Note that you defaultly play as white, but it is possible to change with a minor edit.
- Enter moves as coordinate changes: e2e4, e4e5, etc.
<img width="313" alt="chessboard" src="https://github.com/user-attachments/assets/1276097a-294d-4b2e-b1fa-a4fd26ee5e09">
