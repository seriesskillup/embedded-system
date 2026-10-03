# Chapter 12 — Input / Output (runnable demo)

What it shows and how to run it are in the book, Chapter 12 — Input / Output *Putting it together*.
Covers: printf sends text bytes; specifier vs length modifier; PRIu32; a float argument promoted to double. Runs as-is. The UART retarget hook is hardware-specific and only noted.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
