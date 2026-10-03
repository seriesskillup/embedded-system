# Chapter 8 — Pointers, volatile, MMIO (runnable demo)

What this shows and how to run it are described in the book, Chapter 8,
*Putting it together* (§8.8). In short: address vs content, arithmetic scaled by
type, `*p++` vs `(*p)++`, the `volatile uint32_t *const` register pointer, the
read-modify-write lost-update and its set-register fix, and a dispatch table.

The "registers" are backed by a `sim_regs[]` RAM array so the code runs on a
host (OnlineGDB, etc.); on real silicon the pointer targets the datasheet
address instead.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```
