# Chapter 10 — Strings (runnable demo)

What it shows and how to run it are in the book, Ch10 *Putting it together*.
Covers: string literal (read-only) vs char array (writable); `sizeof` vs
`strlen` and the terminator; a character constant being int-wide; and unbounded
`strcpy` overflow vs bounded `snprintf`. Runs as-is.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
