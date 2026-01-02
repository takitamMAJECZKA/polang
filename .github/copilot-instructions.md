# Copilot Instructions for `mylang` Project

## Project Overview
This is a custom interpreter for "mylang", a programming language with Polish keywords, implemented in a single C file (`main.c`). The project includes a lexer, a Pratt parser, and a tree-walk evaluator.

## Architecture & Core Components
- **Single-File Implementation**: The entire interpreter logic resides in `main.c`.
- **Pipeline**: Source Code -> Lexer (`lex`) -> Tokens -> Parser (`parse`) -> AST (`Node`) -> Evaluator (`eval`).
- **Data Structures**:
  - `Node`: AST node with a tagged union structure (`NodeType`).
  - `Token`: Represents lexical units (`TokenType`).
  - `Variable`: Symbol table entry (`VarType`).
- **Global State**: The project relies heavily on global variables (`tokens`, `variables`, `pos`, `token_count`).

## Language Details (mylang)
- **Keywords (Polish)**: `zmienna` (var), `pisz` (print), `jeżeli` (if), `wkółko` (loop), etc.
- **Syntax**:
  - Variable Declaration: `zmienna <name> = <expression>;`
  - String Concatenation: Supported via `+` operator.
- **Types**: Supports `double` (numbers) and `string` (text).

## Developer Workflow
- **Build**: Compile directly using GCC/Clang.
  ```bash
  gcc main.c -o benzin.exe
  ```
- **Run**: Execute the binary with a source file.
  ```bash
  ./benzin.exe plik.mylang
  ```
- **Debug**: Use `printf` for debugging. The code already contains debug prints for tokenization and evaluation results.

## Coding Conventions & Patterns
- **Memory Management**: Manual `malloc`/`free`. Ensure `free_node` is called to prevent leaks.
- **Fixed Limits**: Be aware of hardcoded limits:
  - `MAX_TOKENS` (256)
  - `MAX_VARS` (32)
  - Source file buffer (1024 bytes)
- **Error Handling**: Errors are printed to stdout (`printf("Błąd: ...")`) and functions return `NULL` or `0`.
- **Naming**:
  - C Code: English (e.g., `Node`, `eval`, `parse`).
  - MyLang Keywords: Polish (e.g., `zmienna`).

## Key Files
- `main.c`: Core implementation (Lexer, Parser, Evaluator, Main).
- `*.mylang`: Example source files for the custom language.
