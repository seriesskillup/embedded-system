/* Deep C for Bare-Metal — Structures, Unions, Enumerations (Chapter 11)
 *
 * Covers: padding and alignment (why sizeof is bigger than the sum of fields);
 * a packed struct; a union used to inspect the bytes of a value; byte order
 * (endianness); and that an enum's constants are int-wide.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

struct plain  { uint8_t a; uint32_t b; uint8_t c; };            /* padded to align b */
struct __attribute__((packed)) packed { uint8_t a; uint32_t b; uint8_t c; };

union bits { float f; uint32_t u; };                            /* same storage, two views */

enum mode { IDLE, RUN, FAULT };                                 /* 0, 1, 2; int-wide */

int main(void)
{
    puts("== Ch11: padding and alignment ==");
    printf("  sizeof(struct plain)  = %zu (fields are 1+4+1=6, padded up)\n", sizeof(struct plain));
    printf("  offsetof(plain, b)    = %zu (b is aligned to a 4-byte boundary)\n",
           offsetof(struct plain, b));
    printf("  sizeof(struct packed) = %zu (packed: no padding)\n", sizeof(struct packed));
    /* Packed is safe to READ, but taking &packed.b gives a possibly-unaligned
     * pointer; dereferencing that faults on a Cortex-M0. Prefer natural layout. */

    puts("\n== Ch11: union type-punning (reading the bytes of a float) ==");
    union bits x;
    x.f = 1.5f;                       /* write the float member ... */
    printf("  1.5f has bit pattern 0x%08X (read back through the uint32_t member)\n", x.u);
    /* In C, reading a different union member than the one written is defined and
     * is the idiomatic way to see a value's bytes. (Not defined in C++.) */

    puts("\n== Ch11: endianness ==");
    uint32_t v = 0x11223344u;
    uint8_t *p = (uint8_t *)&v;       /* look at v one byte at a time */
    printf("  0x11223344 in memory: %02X %02X %02X %02X\n", p[0], p[1], p[2], p[3]);
    printf("  -> this host is %s-endian\n", (p[0] == 0x44u) ? "little" : "big");
    /* The byte order is fixed by the target. Detect and convert at every wire or
     * flash boundary; do not assume the host's order. */

    puts("\n== Ch11: an enum constant is int-wide ==");
    enum mode m = RUN;
    printf("  RUN = %d, sizeof(RUN) = %zu (the constant has type int)\n", m, sizeof RUN);
    printf("  sizeof(enum mode) = %zu (the enum type's size is implementation-defined)\n",
           sizeof(enum mode));

    return 0;
}
