# Chapters 4-5 — Numbers on MCUs (runnable demo)

One project for integers and floating point together. What it shows and how to
run it are in the book: Ch4 and Ch5 point here from their *Putting it together*.

Covers: integer promotion, signed/unsigned comparison, unsigned wraparound, and
truncation on store (Ch4); float inexactness, limited precision, float vs
double, and the printf-float note (Ch5). Runs as-is.

It intentionally triggers `-Wsign-compare` (the `-1 < 1u` trap); the warning is
the lesson.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
