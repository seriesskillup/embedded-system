# Chapters 1-3 — Fundamentals (runnable demo)

One project for the three foundational chapters, so it reads as a complete
program rather than three thin demos. What it shows and how to run it are in the
book: Ch1, Ch2, and Ch3 each point here from their *Putting it together*.

Covers: declaration placement and static vs indeterminate values (Ch1); switch
fall-through and control flow (Ch2); evaluation order, short-circuit, and
sequence points (Ch3). Runs as-is on a hosted compiler.

It intentionally triggers `-Wuninitialized` (reading an uninitialized local),
the compiler catching that exact trap; the program still runs.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
