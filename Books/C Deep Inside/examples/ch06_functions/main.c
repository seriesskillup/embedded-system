/* C Deep Inside for Embedded System Development: Bare-Metal — Functions (Chapter 6, runnable demo)
 *
 * Covers: the call stack and recursion depth (each call costs a frame);
 * reentrancy (a static inside a function breaks it, a local keeps it safe);
 * pass-by-value vs pass-by-pointer; and why small hot functions are inlined.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>

/* Recursion: each level adds a stack frame. Fine when the depth is bounded and
 * small; on an MCU with a few KB of stack, deep or data-dependent recursion can
 * silently overflow into other RAM. Keep it bounded. */
static unsigned sum_to(unsigned n)
{
    if (n == 0u) return 0u;              /* base case bounds the depth */
    return n + sum_to(n - 1u);
}

/* NOT reentrant: the static accumulator is shared by every caller and every
 * interrupt. Two overlapping users corrupt each other's running total. */
static long running_total_bad(int x)
{
    static long acc = 0;                 /* one shared copy, hidden state */
    acc += x;
    return acc;
}

/* Reentrant: the caller owns the state, so overlapping calls do not interfere. */
static long running_total_ok(long *acc, int x)
{
    *acc += x;
    return *acc;
}

/* Pass by value: the caller's variable is unchanged. */
static void add_by_value(int v)   { v += 100; (void)v; }
/* Pass by pointer: the caller's variable is changed through its address. */
static void add_by_pointer(int *v) { *v += 100; }

int main(void)
{
    puts("== Ch6: recursion depth (bounded) ==");
    printf("  sum_to(5) = %u  (5 nested frames, then unwound)\n", sum_to(5));

    puts("\n== Ch6: reentrancy ==");
    /* Two independent users each want their OWN running total, interleaved.
     * (Computed in separate statements: argument order would be unspecified.) */
    long ra1 = running_total_bad(10);    /* user A: expects 10 */
    long rb1 = running_total_bad(1);     /* user B: expects 1  */
    long ra2 = running_total_bad(20);    /* user A: expects 30 */
    printf("  shared static (bad): userA = %ld then %ld, userB = %ld\n", ra1, ra2, rb1);
    printf("    -> userB got %ld (wanted 1) and userA's total is wrong: the static is shared.\n", rb1);

    long a_acc = 0, b_acc = 0;           /* each user owns its own accumulator */
    long oa1 = running_total_ok(&a_acc, 10);
    long ob1 = running_total_ok(&b_acc, 1);
    long oa2 = running_total_ok(&a_acc, 20);
    printf("  caller-owned (ok):   userA = %ld then %ld, userB = %ld  (independent, correct)\n",
           oa1, oa2, ob1);

    puts("\n== Ch6: value vs pointer ==");
    int x = 1;
    add_by_value(x);
    printf("  after add_by_value(x):   x = %d  (unchanged)\n", x);
    add_by_pointer(&x);
    printf("  after add_by_pointer(&x): x = %d  (changed)\n", x);

    /* Inline note: a tiny hot helper marked 'static inline' lets the compiler
     * drop the call overhead, trading a little code size for speed. */
    return 0;
}
