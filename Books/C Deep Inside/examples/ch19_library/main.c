/* Deep C for Bare-Metal — Library Functions (Chapter 19)
 *
 * Covers: memmove for overlapping regions (memcpy is undefined there); why
 * memcmp is a poor struct comparison (it includes padding bytes); a bounded
 * snprintf; and strtok's hidden static state. Freestanding-vs-hosted and the
 * math notes are language facts, summarized in comments.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(void)
{
    puts("== Ch19: memmove handles overlap; memcpy does not ==");
    /* buf = ABCDEF. Copy 4 bytes from src = buf to dst = buf+2. The source range
     * buf[0..3] and the destination buf[2..5] overlap at C and D. */
    char mm[] = "ABCDEF";
    memmove(&mm[2], &mm[0], 4);        /* defined for overlap */
    printf("  memmove(buf+2, buf, 4): \"%s\"  (correct: ABABCD)\n", mm);

    /* A naive FORWARD byte copy (what memcpy MAY do on this overlap) writes each
     * destination byte before the source byte at that spot is read, so C and D
     * are lost. Shown with a manual loop, NOT memcpy: calling memcpy on
     * overlapping regions is undefined. */
    char fc[] = "ABCDEF";
    for (size_t i = 0; i < 4; i++) fc[2 + i] = fc[0 + i];
    printf("  forward copy (like memcpy): \"%s\"  (wrong: ABABAB, C and D lost)\n", fc);

    puts("\n== Ch19: memcmp on structs includes padding ==");
    struct s { uint8_t a; uint32_t b; };   /* has padding between a and b */
    struct s x, y;
    memset(&x, 0, sizeof x); memset(&y, 0xFF, sizeof y);  /* fill padding differently */
    x.a = 1; x.b = 2; y.a = 1; y.b = 2;   /* same field values ... */
    printf("  fields equal, but memcmp(&x,&y) = %d (nonzero: padding differs)\n",
           memcmp(&x, &y, sizeof x));
    printf("  field-by-field equal? %s (the right way)\n",
           (x.a == y.a && x.b == y.b) ? "yes" : "no");

    puts("\n== Ch19: bounded formatting ==");
    char out[8];
    /* The compiler may warn -Wformat-truncation here because it can see the string
     * will not fit. That warning IS the truncation being flagged; it is safe,
     * snprintf never overflows out[8]. */
    int n = snprintf(out, sizeof out, "id=%s", "123456789");
    printf("  snprintf -> \"%s\" (truncated safely), wanted %d chars\n", out, n);

    puts("\n== Ch19: strtok keeps hidden static state ==");
    char csv[] = "a,b,c";
    char *tok = strtok(csv, ",");      /* first call passes the string ... */
    printf("  tokens:");
    while (tok) { printf(" %s", tok); tok = strtok(NULL, ","); }  /* ... then NULL to continue */
    printf("\n  (that hidden state makes strtok non-reentrant; use strtok_r in ISRs/tasks)\n");

    /* Freestanding vs hosted: the stdint, stddef, and mem/str headers are always
     * available; file I/O, malloc, exit, and time assume an OS you do not have.
     * In the math header use the f-suffixed calls (sinf, sqrtf) to stay
     * single-precision. */
    return 0;
}
