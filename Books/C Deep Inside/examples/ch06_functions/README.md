# Chapter 6 — Functions (runnable demo)

What it shows and how to run it are in the book, Ch6 *Putting it together*.
Covers: bounded recursion and the call stack; reentrancy (a shared `static`
breaks it, caller-owned state keeps it safe, shown with two interleaved users);
and pass-by-value vs pass-by-pointer. Runs as-is.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
