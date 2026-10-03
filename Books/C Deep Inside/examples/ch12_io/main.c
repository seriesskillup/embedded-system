/* Deep C for Bare-Metal — Input / Output (Chapter 12)
 *
 * Covers: printf turns a value into TEXT bytes (not the raw value); the same
 * value under different specifiers; the length modifier changing the argument
 * width; and the fixed-width PRIu32 form. The retarget hook (_write / putchar)
 * is hardware-specific and only noted here.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main(void)
{
    puts("== Ch12: printf sends TEXT, chosen by the specifier ==");
    int v = 1234;
    printf("  %%d of 1234 -> \"%d\"  (4 text bytes '1','2','3','4', not the raw int)\n", v);
    printf("  %%x -> \"%x\"   %%o -> \"%o\"   (same value, different rendering)\n", v, v);
    printf("  %%c of 65 -> \"%c\"  (the byte 65 shown as its character)\n", 65);

    puts("\n== Ch12: the length modifier changes the argument width ==");
    /* %hhu reads the argument as unsigned char: 1234 & 0xFF = 210. */
    printf("  %%hhu of 1234 -> \"%hhu\"  (1234 truncated to 8 bits = 210)\n", 1234);
    printf("  %%u   of 1234 -> \"%u\"\n", 1234u);

    puts("\n== Ch12: print a fixed-width type portably ==");
    uint32_t ticks = 100000u;
    printf("  ticks = %" PRIu32 " (PRIu32 picks the right length modifier per target)\n", ticks);

    puts("\n== Ch12: a float argument is promoted to double ==");
    float t = 3.5f;
    printf("  %%f of 3.5f -> \"%f\"  (the float was promoted to double in the call)\n", (double)t);

    /* On an MCU, printf reaches the UART through a retarget hook: printf -> the C
     * library -> _write() (or putchar()), which you implement to push bytes out
     * the UART. That hook is device-specific, so it is not shown here. If %f
     * prints nothing on your toolchain, link with -u _printf_float. */
    return 0;
}
