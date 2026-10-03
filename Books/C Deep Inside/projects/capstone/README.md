# Capstone — mock sensor node (runnable demo)

One short program that exercises the whole book at once: read a temperature,
decide a fan duty, drive a register, log the sample. It is not a driver for any
real chip — it is the *shape* of one, written so you can read each line and say
what it costs and does on the target.

What each piece ties back to:

| Piece | Chapter |
|---|---|
| Q8.8 fixed-point temperature (no float) | 4, 5 |
| `volatile` memory-mapped registers via a pointer | 8 |
| `union` overlay of the register block (named fields vs raw words) | 11 |
| status flags packed in one register, set/cleared with masks | 13 |
| `typedef` names for the register and fixed-point types | 14 |
| `const` calibration + per-mode gain tables in "flash" | 15 |
| ring buffer (array + index wrapped with a bitwise mask) | 9 |

As in the Chapter 8 demo, the "registers" are backed by a RAM block so this runs
on a host (OnlineGDB, etc.). On silicon you point `DEV` at the datasheet base
address instead; nothing else changes.

```
gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
```

Try it yourself: change `mode` to `MODE_ECO` or `MODE_TURBO` and watch the fan
duty respond; adjust the `adc_samples[]` sweep; move `SETPOINT_Q`.
