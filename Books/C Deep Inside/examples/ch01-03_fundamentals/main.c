/* Deep C for Bare-Metal — Fundamentals (Chapters 1-3, runnable demo)
 *
 * Ch1 Declarations: where things live, zero-init vs indeterminate, static lifetime.
 * Ch2 Control:      switch fall-through, bounded loops, the assignment-in-if trap.
 * Ch3 Expressions:  unspecified argument order, short-circuit, a sequence-point note.
 *
 * Runs as-is on a hosted GCC (OnlineGDB, etc.):
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>

/* --- Ch1: declarations and where they live --- */
static int g_zero;                       /* static storage: starts at 0, no init needed */
static const uint8_t lut[4] = { 1, 2, 4, 8 };  /* const -> .rodata (flash on an MCU), read-only */

static int next_id(void)
{
    static int counter = 0;              /* static local: keeps its value across calls */
    return ++counter;
}

/* --- Ch3: prove argument evaluation order is UNSPECIFIED --- */
static int step(const char *tag)
{
    static int n = 0;
    printf("    evaluated %s (call #%d)\n", tag, ++n);
    return n;
}

int main(void)
{
    puts("== Ch1: declarations ==");
    int local_uninit;                    /* automatic: value is INDETERMINATE, not 0 */
    printf("  g_zero (static)      = %d  (zero-initialized)\n", g_zero);
    printf("  lut[3] (const/flash) = %u  (read in place)\n", lut[3]);
    printf("  next_id() x3         = %d %d %d  (static local persists)\n",
           next_id(), next_id(), next_id());
    /* The next line reads an uninitialized local. It INTENTIONALLY triggers
     * -Wuninitialized ("'local_uninit' is used uninitialized"), the compiler
     * catching exactly this. On an MCU the value is whatever stack garbage was there. */
    printf("  local_uninit         = %d  (indeterminate; on an MCU this is stack garbage)\n",
           local_uninit);
    /* On the target: g_zero -> .bss, lut -> .rodata/flash, local_uninit -> stack.
     * Writing through a cast to lut (e.g. ((uint8_t*)lut)[0] = 9) is undefined and
     * faults on real hardware because flash is not writable. */

    puts("\n== Ch2: control ==");
    for (int mode = 0; mode < 3; mode++) {
        printf("  mode %d: ", mode);
        switch (mode) {
        case 0:
            printf("zero ");
            /* fall through */           /* intentional: no break, so case 1 runs too */
        case 1:
            printf("one\n");
            break;
        default:
            printf("other\n");
            break;
        }
    }
    /* TRY THIS to see a real warning:  if (g_zero = 1) { ... }
     * '=' (assign) where '==' (compare) was meant. -Wall warns:
     *   "suggest parentheses around assignment used as truth value" (-Wparentheses). */

    puts("\n== Ch3: expressions ==");
    /* The comma operator IS a sequence point, so these run strictly left to right: */
    printf("  comma operator runs in order:\n");
    (void)step("a"), (void)step("b");
    /* But function ARGUMENT order is UNSPECIFIED. printf("%d %d", step(), step())
     * may evaluate the two calls in either order, so never rely on it. */
    int r = 0;
    if (1 || (r = 99)) { }               /* short-circuit: the right side is not evaluated */
    printf("  short-circuit: r stayed %d (|| skipped the right side)\n", r);
    /* Sequence-point note: 'i = i++;' is UNDEFINED (two writes, no sequence point).
     * We do not run it; the point is to never write it. */

    return 0;
}
