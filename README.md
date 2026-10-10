# Compiler Construction Project

## Overview

This repository contains the implementation of **Stage 1** for the Compiler Construction project. It features a fully functional **Lexical Analyzer (Lexer)** and a **Table-Driven LL(1) Syntax Analyzer (Parser)** built entirely from scratch in C.

The compiler processes source code written in a custom procedural programming language, validates its syntax against a strict LL(1) grammar, and dynamically generates an N-ary Parse Tree.

---

## Key Features

### 🔹 Lexical Analysis (DFA-based)

* Implements a highly efficient Lexer using a **Twin Buffer** architecture (`BUFFER_SIZE = 50`) to minimize disk I/O overhead.
* Strictly enforces custom regex constraints for:

  * regular variables (`TK_ID`)
  * record fields (`TK_FIELDID`)
  * record names (`TK_RUID`)
  * real numbers (`TK_RNUM`)

### 🔹 Syntax Analysis (LL(1) Predictive Parser)

* Uses a stack-based predictive parsing engine driven by pre-computed `FIRST` and `FOLLOW` sets.

### 🔹 Robust Error Recovery (Panic Mode)

* Incorporates textbook LL(1) Panic Mode recovery using synchronization tokens (`;`, `end`, `endif`, `endwhile`).
* Gracefully discards invalid token sequences.
* Suppresses cascading/duplicate errors.
* Tracks line numbers accurately across multi-line statements.

### 🔹 N-ary Parse Tree Generation

* Dynamically builds an Abstract Syntax Tree (AST) using a **First-Child / Next-Sibling** pointer architecture.
* Exports the tree using **In-Order traversal**
  *(Leftmost child → Parent → Remaining siblings)*.
* Output strictly matches the required 7-column tabular format.

### 🔹 Performance Tracking

* Includes execution time measurement using CPU clock ticks.

---

## File Structure

| File                 | Description                                           |
| -------------------- | ----------------------------------------------------- |
| `driver.c`           | Main interactive menu loop and file I/O handling      |
| `lexer.c/.h/Def.h`   | Twin Buffer, DFA state machine, token generation      |
| `parser.c/.h/Def.h`  | Stack ADT, Tree Node ADT, Parse Table, Parsing Engine |
| `makefile`           | Automated build script                                |
| `t1.txt` – `t10.txt` | Test cases (valid and erroneous)                      |

---

## How to Build & Run

### 1. Compile the Project

Ensure `gcc` and `make` are installed. From the project directory:

```bash
make clean
make
```

---

### 2. Execute the Compiler

Run the generated executable:

```bash
./stage1exe t1.txt parsetreeOutFile.txt
```

---

## Main Menu Options

| Option | Description                                 |
| ------ | ------------------------------------------- |
| **0**  | Exit the compiler                           |
| **1**  | Remove comments (`%`) and print clean code  |
| **2**  | Run lexical analysis and print token stream |
| **3**  | Run syntax analysis and generate parse tree |
| **4**  | Display total CPU execution time            |

---

## Error Handling

The compiler explicitly handles both:

* **Lexical Errors**

  * unknown symbols
  * identifier length violations

* **Syntax Errors**

  * terminal mismatches
  * non-terminal mismatches

Errors are reported sequentially with accurate line numbers—even for multi-line statements—without crashing the program.
