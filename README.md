# Caissa - Chess Engine

A compact chess engine written in C++ that uses a JSON configuration for board representation, move generation and evaluation.

**Features**

- Simple console play loop (human vs engine)
- FEN-based initial position read from [settings.json](settings.json)
- Move generation and minimax-style recursive search (fixed depth in `play()`)
- Piece-square tables and configurable piece weights via JSON

**Requirements**

- C++ compiler (g++/MinGW recommended on Windows)
- Single-header JSON library `json.hpp` (nlohmann) present in the project root
- Windows console: code sets UTF-8 output code page (`SetConsoleOutputCP(CP_UTF8)`) for piece glyphs

**Files**

- [engine_final.cpp](engine_final.cpp) — main engine source and console UI
- [engine.cpp](engine.cpp) — alternate/older source (kept for reference)
- [json.hpp](json.hpp) — nlohmann JSON single-header (required)
- [settings.json](settings.json) — engine configuration (FEN, move directions, weights, etc.)
- sum.py — small helper script (if present)

**Build**

- Using g++ (example):

```bash
g++ -std=c++17 engine_final.cpp -o engine.exe
```

- Or use the provided VS Code build task: run the `C/C++: g++.exe build active file` task while `engine_final.cpp` is the active editor.

**Run**

- Ensure `settings.json` and `json.hpp` are in the same directory as the binary.
- From PowerShell or CMD:

```powershell
.\engine.exe
```

- The program prints the board using Unicode piece glyphs and prompts:

```
   your move:
```

- Enter moves in coordinate notation, e.g. `e2e4` (source square + target square). The engine will play a reply.

**Configuration (`settings.json`)**
The engine is driven by the JSON file:

- `fen` — starting FEN string. The engine reads the side-to-move from the FEN.
- `directions` — movement offsets per piece (engine uses a 120-square board representation; offsets are indexed accordingly).
- `colors` — numeric color mapping for pieces (0 = white, 1 = black).
- `weights` — piece values used in evaluation.
- `pst` — piece-square table (120 entries) used in evaluation.
- `rank_2`, `rank_7` — lists of square indices used to detect initial pawn ranks for double-push and promotion logic.
- `coordinates` — mapping of 120-board indices to algebraic coordinates (used for input/output)
- `pieces` — printable glyphs used when rendering the board

Modify `settings.json` to experiment with different evaluation parameters or to load a custom FEN position.

**How the engine works (brief)**

- Board representation: the code converts a FEN into a padded 120-element string board and uses offset-based movement (classic 10x12 mailbox).
- Move generation: `genMoves()` walks piece offsets to build pseudo-legal moves (no full check legality filtering).
- Search: recursive negamax-style search with fixed depth (depth is currently hard-coded in `play()` where `search(3)` is called).
- Evaluation: material + PST values with sign adjusted by side to move.

**Limitations & Known Issues**

- No castling or full check legality detection; move generation may include illegal moves that are not filtered by check.
- En passant and some promotion edge-cases may be incomplete depending on the source file version.
- Input validation is minimal; malformed input may crash or behave unexpectedly.
- Single-threaded and intended for small depths; not tuned for performance.

**Development & Experimentation**

- To change search depth, edit the `search(...)` call used in `play()` (look for `search(3)`), rebuild and run.
- To add features (castling, en passant, promotion choices, quiescence, alpha-beta), search the repository for `genMoves`, `makeMove`, and `takeBack` and implement state changes carefully (store historic state in the move structure to allow undo).

**Contributing**

- This project is educational — feel free to open issues or submit PRs for bug fixes and features. Keep changes small and focused.

**License**

- MIT License — copy/adapt as needed.

---
