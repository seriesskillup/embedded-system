/* C Deep Inside for Embedded System Development: Bare-Metal — Numbers on MCUs (Chapters 4-5, runnable demo)
 *
 * Ch4 Integers: promotion to int, signed/unsigned comparison, unsigned wraparound,
 *               truncation when you store back into a narrow type.
 * Ch5 Floating: float is not exact, limited precision, float vs double, and the
 *               "prints nothing" printf-float note for MCUs.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void)
{
    puts("== Ch4: integer promotion ==");
    uint8_t a = 200, b = 100;
    /* a + b: both promote to int, so the sum is 300, computed in int, no wrap. */
    int sum_wide = a + b;
    printf("  a + b evaluated as int = %d  (no wrap: computed wide)\n", sum_wide);
    /* But storing back into a uint8_t truncates to 8 bits: 300 & 0xFF = 44. */
    uint8_t narrow = (uint8_t)(a + b);   /* explicit cast: the truncation is the point */
    printf("  stored back in uint8_t = %u  (300 truncated to 8 bits)\n", narrow);

    puts("\n== Ch4: signed vs unsigned comparison ==");
    int s = -1;
    unsigned u = 1u;
    /* -1 converts to a huge unsigned before the compare, so -1 < 1u is FALSE. */
    /* This comparison intentionally triggers -Wextra's warning at build time:
     * "comparison of integer expressions of different signedness" (-Wsign-compare).
     * The warning is the lesson; the program still runs. */
    printf("  (-1 < 1u) is %s  (the -1 became UINT_MAX)\n", (s < u) ? "true" : "false");

    puts("\n== Ch4: unsigned wraparound ==");
    unsigned z = 0u;
    printf("  0u - 1 = %u  (wraps to UINT_MAX = %u)\n", z - 1u, UINT_MAX);

    puts("\n== Ch5: floating point is not exact ==");
    float f = 0.1f + 0.2f;
    printf("  0.1f + 0.2f = %.8f  (not exactly 0.3)\n", (double)f);
    printf("  (0.1f + 0.2f == 0.3f) is %s\n", (0.1f + 0.2f == 0.3f) ? "true" : "false");

    puts("\n== Ch5: float precision runs out ==");
    float big = 16777217.0f;             /* 2^24 + 1: not representable as float */
    printf("  16777217.0f stored as float = %.1f  (lost the +1; ~7 digits only)\n",
           (double)big);

    puts("\n== Ch5: float vs double on an MCU ==");
    printf("  sizeof(float) = %zu, sizeof(double) = %zu\n", sizeof(float), sizeof(double));
    /* Note for the target: every double operation may be a soft-float library call
     * on an FPU-less part, and printing a float can need a bigger printf. If %f
     * prints nothing on your toolchain, add the linker flag -u _printf_float. */

    return 0;
}
