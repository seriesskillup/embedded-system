/* Deep C for Bare-Metal — Complicated Declarations (Chapter 18)
 *
 * Covers: the right-left rule on real forms; the parentheses that separate a
 * function returning a pointer from a pointer to a function; the three confused
 * shapes; and how a typedef turns an "unreadable" declaration readable. The
 * declarations are exercised so you can see they mean what the rule says.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>

static int square(int x) { return x * x; }

/* int (*pf)(int): a POINTER to a function taking int, returning int. */
/* int *fp(int):   a FUNCTION taking int, returning int* (different thing). */

/* signal()'s shape, made readable with typedefs (Ch18 §18.4). */
typedef void (*sig_handler_t)(int);          /* the callback parameter */
typedef int  (*result_fn_t)(int);            /* what the returned pointer points at */
static int  handler_ret(int code) { return code + 1; }
static result_fn_t register_handler(int sig, sig_handler_t h) { (void)sig; (void)h; return handler_ret; }
static void my_handler(int s) { (void)s; }

int main(void)
{
    puts("== Ch18: pointer to function vs function returning pointer ==");
    int (*pf)(int) = square;                 /* pointer to function */
    printf("  pf(6) via function pointer = %d\n", pf(6));

    puts("\n== Ch18: the three confused shapes ==");
    char *words[2] = { "led", "reset" };     /* array of 2 pointers to char */
    char  grid[2][3] = { {'a','b','c'}, {'d','e','f'} };  /* contiguous 2x3 block */
    printf("  words[1] = \"%s\"  (array of pointers)\n", words[1]);
    printf("  grid[1][2] = '%c'  (one contiguous block)\n", grid[1][2]);
    printf("  sizeof(words) = %zu (two pointers), sizeof(grid) = %zu (6 chars)\n",
           sizeof words, sizeof grid);

    puts("\n== Ch18: the 'unreadable' signal(), made readable by typedef ==");
    result_fn_t prev = register_handler(2, my_handler);   /* reads cleanly now */
    printf("  returned function pointer prev(41) = %d\n", prev(41));
    return 0;
}
