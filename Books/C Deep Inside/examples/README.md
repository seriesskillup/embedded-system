# Deep C for Bare-Metal — Runnable Examples

One self-contained project per chapter. Each `chNN_topic/` folder holds a
commented `main.c` (multiple files only where multi-file *is* the lesson) that
gathers the chapter's key points into code you can compile and run.

**The full "how to run" explanation is in the book, Chapter 0 ("The runnable
examples"), and each project's purpose is described in that chapter's *Putting
it together* section.** This file is just the repository index.

Quick run (web IDE such as OnlineGDB, or your machine):

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```

Two kinds of project: most run as-is; the hardware chapters (8, and parts of 11
and 16) back their "registers" with a small RAM array so they run on a host too.

## Index

| Chapter(s) | Folder | Notes |
|---|---|---|
| 1-3 — Declarations, Control, Expressions | `ch01-03_fundamentals/` | grouped; intentional `-Wuninitialized` |
| 4-5 — Integers, Floating point | `ch04-05_numbers/` | grouped; intentional `-Wsign-compare` |
| 6 — Functions | `ch06_functions/` | runs as-is |
| 7 — The Preprocessor | `ch07_preprocessor/` | runs as-is |
| 8 — Pointers, volatile, MMIO | `ch08_pointers/` | simulated registers |
| 9 — Arrays | `ch09_arrays/` | runs as-is; intentional `-Wsizeof-array-argument` |
| 10 — Strings | `ch10_strings/` | runs as-is |
| 11 — Structures, Unions, Enums | `ch11_structs/` | runs as-is |
| 12 — Input / Output | `ch12_io/` | runs as-is |
| 13 — Bitwise Operators | `ch13_bitwise/` | intentional `-Wparentheses`, `-Wconversion` |
| 14 — typedef | `ch14_typedef/` | runs as-is |
| 15 — const & Read-only Data | `ch15_const/` | simulated status register |
| 16 — Dynamic Memory Allocation | `ch16_memory/` | runs as-is (host malloc) |
| 17 — Variable Number of Arguments | `ch17_variadics/` | runs as-is |
| 18 — Complicated Declarations | `ch18_declarations/` | runs as-is |
| 19 — Library Functions | `ch19_library/` | intentional `-Wformat-truncation` |
