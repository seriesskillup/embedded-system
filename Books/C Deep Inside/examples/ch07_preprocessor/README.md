# Chapter 7 — The Preprocessor (runnable demo)

What it shows and how to run it are in the book, Ch7 *Putting it together*.
Covers: the missing-parentheses trap; multiple evaluation of macro arguments
with side effects; the `do/while(0)` idiom; and preferring `enum`/`const` over
`#define` for constants. Runs as-is.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
