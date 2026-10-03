# Chapter 19 — Library Functions (runnable demo)

What it shows and how to run it are in the book, Chapter 19 — Library Functions *Putting it together*.
Covers: memmove vs memcpy on overlap, memcmp including padding, bounded snprintf, and strtok's hidden static state. One intentional warning (-Wformat-truncation).

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
