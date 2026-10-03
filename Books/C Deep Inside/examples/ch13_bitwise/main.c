/* C Deep Inside for Embedded System Development: Bare-Metal — Bitwise Operators (Chapter 13)
 *
 * Covers: set / clear / toggle / test a bit; extract a multi-bit field with
 * mask-and-shift; insert a field (clear then OR the value in position); the
 * precedence trap; and why a signed 1-bit field holds 0 and -1.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    puts("== Ch13: set / clear / toggle / test a bit ==");
    uint8_t r = 0x00u;
    r |=  (uint8_t)(1u << 3);          /* set bit 3   */
    printf("  after set bit 3:    0x%02X\n", r);
    r &= (uint8_t)~(1u << 3);          /* clear bit 3 */
    printf("  after clear bit 3:  0x%02X\n", r);
    r ^=  (uint8_t)(1u << 5);          /* toggle bit 5 */
    printf("  after toggle bit 5: 0x%02X\n", r);
    printf("  bit 5 is %s\n", (r & (1u << 5)) ? "set" : "clear");

    puts("\n== Ch13: extract a field (mask and shift) ==");
    uint8_t reg = 0xADu;               /* 1010 1101 */
    /* pull out bits [5:4]: shift down, then mask to the field width (2 bits) */
    uint8_t field = (uint8_t)((reg >> 4) & 0x3u);
    printf("  reg=0x%02X, bits[5:4] = %u\n", reg, field);

    puts("\n== Ch13: insert a field (clear then OR value in position) ==");
    uint8_t val = 0x3u;                /* new 2-bit value for bits [5:4] */
    reg = (uint8_t)((reg & ~(0x3u << 4)) | ((val & 0x3u) << 4));  /* clear then set */
    printf("  after inserting 0x3 at [5:4]: reg=0x%02X\n", reg);

    puts("\n== Ch13: the precedence trap ==");
    uint8_t flags = 0x01u;
    /* '==' binds tighter than '&', so (flags & 0x01 == 0x01) means
     * flags & (0x01 == 0x01) = flags & 1. Parenthesize the mask test.
     * The next line intentionally triggers -Wparentheses, the compiler catching
     * exactly this precedence trap. */
    printf("  flags & 0x01 == 0x01  gives %d (surprise: == ran first)\n",
           (flags & 0x01u == 0x01u));
    printf("  (flags & 0x01) == 0x01 gives %d (what you meant)\n",
           ((flags & 0x01u) == 0x01u));

    puts("\n== Ch13: a signed 1-bit field holds 0 and -1 ==");
    struct { int s : 1; unsigned u : 1; } bf = { 0 };
    /* Storing 1 into the signed 1-bit field intentionally triggers -Wconversion
     * ("changes value from 1 to -1"): the field has only a sign bit, so 1 reads
     * back as -1. That is the lesson. */
    bf.s = 1; bf.u = 1;                /* store 1 into each */
    printf("  signed 1-bit reads %d, unsigned 1-bit reads %u\n", bf.s, bf.u);

    /* Bit-banding (Cortex-M3/M4): each bit is also aliased to its own word
     * address, so a single-bit write needs no read-modify-write. That is a
     * hardware memory feature, not a C operator; see Chapter 13's note. */
    return 0;
}
