# Chapter 9 — Arrays (runnable demo)

What this shows and how to run it are described in the book, Chapter 9,
*Putting it together* (§9.5). In short: decay, `sizeof(array)` vs
`sizeof(pointer)`, `arr` vs `&arr`, `a[i]` vs `p[i]`, the `ARRAY_SIZE` idiom, and
a 2-D array decaying to `int (*)[3]`. Runs as-is on any hosted compiler.

It deliberately triggers one warning, `-Wsizeof-array-argument`, the compiler
catching the array-parameter decay trap; the program still runs.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
