# Caissa Chess Engine

Caissa is a lightweight console chess engine written in C++. The project is intentionally compact and data-driven: movement rules, board mapping, piece values, and display glyphs are all provided through JSON configuration.

This README is a full project guide for the current repository state.

## Contents

1. [Project Goals](#project-goals)
2. [Repository Structure](#repository-structure)
3. [How The Engine Works](#how-the-engine-works)
4. [Configuration Schema](#configuration-schema)
5. [Build And Run](#build-and-run)
6. [Input Format](#input-format)
7. [Limitations](#limitations)
8. [Development Notes](#development-notes)
9. [Troubleshooting](#troubleshooting)

## Project Goals

- Provide a playable human-vs-engine command-line chess program.
- Keep implementation understandable for learning and experimentation.
- Use JSON for fast tuning of movement and evaluation behavior.

## Repository Structure

Top-level project files:

- [engine_final.cpp](engine_final.cpp): Main engine implementation and program entry point.
- [settings.json](settings.json): Runtime data for FEN, piece movement offsets, colors, weights, PST, and board coordinate mapping.
- [json.hpp](json.hpp): nlohmann single-header JSON dependency.
- [engine_final.exe](engine_final.exe): Compiled Windows executable currently committed in the repo.
- [README.md](README.md): Project documentation.

Editor/build configuration:

- [.vscode/tasks.json](.vscode/tasks.json): Build task using `C:\MinGW\bin\g++.exe`.
- [.vscode/c_cpp_properties.json](.vscode/c_cpp_properties.json): IntelliSense settings (`compilerPath` currently set to `cl.exe`).
- [.vscode/settings.json](.vscode/settings.json): File associations for C++ editing support.

## How The Engine Works

### High-level flow

1. Load [settings.json](settings.json).
2. Parse side-to-move from the configured FEN.
3. Build an internal mailbox-style board representation.
4. Print board in Unicode chess glyphs.
5. Accept a player move.
6. Search for an engine response at fixed depth.
7. Repeat until the current termination condition is reached.

### Core data structures

- `chess_move`: stores `source`, `target`, `piece`, and `captured`.
- `board` (string): 120-cell mailbox representation with padding and newline delimiters.
- `side`: side to move (`0` white, `1` black).
- `move_history`: history used for undo and limited special-case logic.

### Board model

- Uses a 10x12 mailbox layout flattened into a string.
- Real board squares are represented by piece chars or `.` for empty.
- Padding/offboard boundaries are represented with spaces/newlines.
- Square name translation (for user input and output) is driven by `coordinates` in JSON.

### Move generation

`genMoves()` builds pseudo-legal moves:

- Sliding pieces iterate along configured direction offsets.
- Leapers (kings, knights, pawns in this implementation) stop after one step.
- Friendly squares block movement.
- Captures are included and then stop on that ray.
- Pawn movement includes directional push/capture handling and initial two-square push checks.

Important: the generator does not fully enforce chess legality with king-safety validation.

### Make/unmake

- `makeMove(...)` applies move, flips side, and pushes history.
- `takeBack(...)` restores source/target squares, flips side, and pops history.
- Promotion is currently auto-queen using rank arrays from config.

### Evaluation

`eval()` computes score using:

- Material via `weights`.
- Positional adjustments via `pst` (piece-square table).
- Sign adjustment based on side-to-move.

### Search

- Negamax recursion with alpha-beta pruning.
- Root search records best source/target in global members.
- Gameplay currently uses fixed depth `searchRoot(3)`.

## Configuration Schema

All required engine settings are in [settings.json](settings.json):

- `fen`: Initial FEN string; side-to-move is parsed from this.
- `directions`: Per-piece move offsets for mailbox board indexing.
- `colors`: Piece-to-color map.
- `weights`: Material values used by evaluation.
- `pst`: 120-entry table used for positional scoring.
- `rank_2`, `rank_7`: Pawn start-rank index arrays.
- `coordinates`: Index-to-algebraic mapping (`a1`, `e4`, etc.).
- `pieces`: Display mapping for Unicode piece rendering and board symbols.

If any of these keys are missing or malformed, runtime behavior may be incorrect or unstable.

## Build And Run

### Requirements

- Windows (current code includes `windows.h` and uses `SetConsoleOutputCP(CP_UTF8)`).
- C++17 compiler.
- [json.hpp](json.hpp) present in project root.
- [settings.json](settings.json) present in working directory when running.

### Build using VS Code task

Use task defined in [.vscode/tasks.json](.vscode/tasks.json):

- `C/C++: g++.exe build active file`

Make sure [engine_final.cpp](engine_final.cpp) is active before running the task.

### Build manually

```powershell
C:\MinGW\bin\g++.exe -std=c++17 -g engine_final.cpp -o engine_final.exe
```

### Run

```powershell
.\engine_final.exe
```

## Input Format

Enter moves as 4-character coordinate strings:

- Example: `e2e4`
- First two chars = source square
- Last two chars = target square

Current input validation is minimal. Invalid or unknown coordinates are not safely rejected in all cases.

## Limitations

Current implementation is educational and not a full tournament-compliant engine.

1. No castling support.
2. En passant logic is incomplete.
3. No full king-safety legality filtering for all pseudo-legal moves.
4. Promotion is fixed to queen.

### Fast tuning points

- Change engine strength/speed by adjusting `searchRoot(3)` depth in [engine_final.cpp](engine_final.cpp).
- Tune style by editing `weights` and `pst` in [settings.json](settings.json).
- Load custom positions by changing `fen` in [settings.json](settings.json).

## Troubleshooting

### Engine fails to start or load position

- Ensure [settings.json](settings.json) exists and contains valid JSON.
- Run executable from project root so relative file loading works.

### Unicode pieces do not render correctly

- Use a UTF-8 capable terminal and font.
- Keep the Windows code-page call in [engine_final.cpp](engine_final.cpp).