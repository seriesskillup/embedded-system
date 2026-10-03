/* Deep C for Bare-Metal — Variable Number of Arguments (Chapter 17)
 *
 * Covers: the <stdarg.h> mechanics; the default argument promotions (read
 * integers as int, floating point as double); the count being the only bound;
 * and the format attribute that makes the compiler type-check a printf-like
 * function.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdarg.h>

/* Sum 'count' ints. The count is the ONLY thing bounding the walk. */
static long sum(int count, ...)
{
    va_list ap; va_start(ap, count);
    long total = 0;
    for (int i = 0; i < count; i++) total += va_arg(ap, int);   /* read each as int */
    va_end(ap);
    return total;
}

/* Average. The caller may pass float literals, but they are promoted to double
 * at the call, so read them back as double, NOT float. */
static double avg(int count, ...)
{
    va_list ap; va_start(ap, count);
    double s = 0.0;
    for (int i = 0; i < count; i++) s += va_arg(ap, double);    /* NOT va_arg(ap, float) */
    va_end(ap);
    return count ? s / count : 0.0;
}

/* A printf-like logger the compiler will type-check, thanks to the attribute
 * (1 = format string parameter, 2 = first variadic argument). */
__attribute__((format(printf, 1, 2)))
static void trace(const char *fmt, ...)
{
    va_list ap; va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

int main(void)
{
    puts("== Ch17: read at the promoted type ==");
    printf("  sum(3, 10, 20, 30) = %ld\n", sum(3, 10, 20, 30));
    printf("  avg(2, 1.5f, 2.5f) = %.2f  (floats were promoted to double)\n", avg(2, 1.5f, 2.5f));

    puts("\n== Ch17: the format attribute type-checks calls ==");
    trace("  trace: id=%d, name=%s\n", 7, "sensor");   /* correct: matches the format */
    /* TRY THIS to see the compiler catch a mismatch at build time:
     *   trace("  x=%d\n", 3.14);   // warning: format '%d' expects int, argument is double
     * That warning (-Wformat, on with -Wall) is what the attribute buys you. */
    return 0;
}
