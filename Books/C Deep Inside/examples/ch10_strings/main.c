/* Deep C for Bare-Metal — Strings (Chapter 10, runnable demo)
 *
 * Covers: a string literal is a read-only char[] (writing it is undefined); a
 * char array is writable; sizeof includes the terminator while strlen does not;
 * a character constant is int-wide; and unbounded strcpy overflows while bounded
 * snprintf does not.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
    puts("== Ch10: literal vs array ==");
    const char *lit = "hi";              /* points into read-only storage (flash on an MCU) */
    char buf[8] = "hi";                  /* a writable copy in RAM */
    buf[0] = 'H';                        /* fine: buf is writable */
    printf("  literal = \"%s\" (read-only), array = \"%s\" (writable)\n", lit, buf);
    /* Writing through the literal, e.g. ((char*)lit)[0] = 'H', is undefined and
     * faults on real hardware because the bytes live in flash. */

    puts("\n== Ch10: sizeof vs strlen, and the terminator ==");
    printf("  sizeof(\"hi\") = %zu  (includes the '\\0')\n", sizeof("hi"));
    printf("  strlen(\"hi\") = %zu  (does not)\n", strlen("hi"));

    puts("\n== Ch10: a character constant is int-wide ==");
    printf("  sizeof('A') = %zu  (a char constant has type int in C)\n", sizeof('A'));
    printf("  sizeof((char)'A') = %zu  (an actual char is 1 byte)\n", sizeof((char)'A'));

    puts("\n== Ch10: unbounded vs bounded copy ==");
    char dst[8];
    const char *src = "1234567890";      /* 10 chars + '\0' = 11 bytes, dst holds 8 */
    /* strcpy(dst, src) would overrun dst by 3 bytes and corrupt whatever is next,
     * with no crash to warn you on a no-MMU MCU. Use a bounded copy instead: */
    int n = snprintf(dst, sizeof dst, "%s", src);   /* truncates safely to fit */
    printf("  snprintf into dst[8] = \"%s\"  (safely truncated)\n", dst);
    printf("  snprintf returned %d  (the length it WANTED to write, before truncation)\n", n);

    return 0;
}
