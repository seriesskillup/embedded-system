# Chapter 17 — Variable Number of Arguments (runnable demo)

What it shows and how to run it are in the book, Chapter 17 — Variable Number of Arguments *Putting it together*.
Covers: the <stdarg.h> mechanics, reading at the promoted type (double, not float), and the format attribute that type-checks a printf-like function. Runs as-is.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
