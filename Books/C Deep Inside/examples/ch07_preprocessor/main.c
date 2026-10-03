/* C Deep Inside for Embedded System Development: Bare-Metal — The Preprocessor (Chapter 7, runnable demo)
 *
 * Covers: text substitution and the missing-parentheses trap; multiple
 * evaluation of macro arguments with side effects; the do/while(0) idiom for a
 * multi-statement macro; and why constants are better as enum/const than macros.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>

/* WRONG: no parentheses. The macro is pure text, so precedence bites. */
#define SQ_BAD(x)  x * x
/* FIX: parenthesize the whole thing and each argument. */
#define SQ_OK(x)  ((x) * (x))

/* Evaluates its argument(s) more than once: dangerous with side effects. */
#define MAX(a, b)  ((a) > (b) ? (a) : (b))

/* A multi-statement macro wrapped so it behaves like one statement. */
#define LOG2(tag, val) do { printf("  %s = %d\n", (tag), (val)); } while (0)

int main(void)
{
    puts("== Ch7: missing parentheses ==");
    /* SQ_BAD(1 + 2) expands to 1 + 2 * 2 = 5, not 9. */
    printf("  SQ_BAD(1 + 2) = %d  (text-substituted to 1 + 2*2)\n", SQ_BAD(1 + 2));
    printf("  SQ_OK(1 + 2)  = %d  (parenthesized, correct)\n", SQ_OK(1 + 2));

    puts("\n== Ch7: multiple evaluation of arguments ==");
    int i = 5, j = 8;
    /* MAX(i++, j): whichever side wins is evaluated twice, so i++ may run twice. */
    int m = MAX(i++, j);
    printf("  MAX(i++, j) = %d, and i is now %d  (i++ was evaluated more than once)\n", m, i);

    puts("\n== Ch7: the do/while(0) idiom ==");
    LOG2("temperature", 42);             /* behaves as a single statement, even before an else */

    puts("\n== Ch7: prefer const/enum over #define for constants ==");
    /* #define has no type and no scope; enum and const are typed and debuggable. */
    enum { BUFFER_SIZE = 64 };           /* typed int constant, visible to the debugger */
    printf("  BUFFER_SIZE (enum) = %d\n", BUFFER_SIZE);
    /* Note: in C++ (and C23) prefer 'constexpr'; see Chapter 20/21. */

    return 0;
}
