/* C Deep Inside for Embedded System Development: Bare-Metal — typedef (Chapter 14)
 *
 * Covers: the typedef-vs-#define pointer trap; naming a function-pointer type;
 * and the opaque-handle pattern (a typedef'd pointer to an incomplete struct,
 * so callers hold a handle but cannot see or touch the internals).
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>

typedef char *str_t;               /* a real type alias for "pointer to char" */
#define STR char *                 /* a text macro; NOT the same thing */

typedef void (*cmd_fn)(int);       /* name a function-pointer type */
static void on_start(int arg) { printf("    on_start(%d)\n", arg); }
static void on_stop(int arg)  { printf("    on_stop(%d)\n", arg);  }

/* Opaque handle: callers see only this pointer type, never the struct body. */
typedef struct device *device_t;   /* 'struct device' is incomplete here */
struct device { int id; int open; };            /* the real definition (would live in the .c) */
static struct device dev_storage;
static device_t device_open(int id) { dev_storage.id = id; dev_storage.open = 1; return &dev_storage; }
static int      device_id(device_t d) { return d->id; }   /* only this module sees the fields */

int main(void)
{
    puts("== Ch14: typedef vs #define with pointers ==");
    str_t a, b;                    /* typedef: BOTH are char* */
    STR   c, d;                    /* macro expands to: char *c, d;  -> d is a plain char! */
    printf("  sizeof a,b (str_t)  = %zu, %zu  (both pointers)\n", sizeof a, sizeof b);
    printf("  sizeof c,d (STR)    = %zu, %zu  (d is a char, not a pointer!)\n", sizeof c, sizeof d);

    puts("\n== Ch14: a named function-pointer type ==");
    cmd_fn table[] = { on_start, on_stop };   /* reads cleanly thanks to the typedef */
    for (size_t i = 0; i < sizeof table / sizeof table[0]; i++)
        table[i]((int)i);

    puts("\n== Ch14: opaque handle ==");
    device_t h = device_open(7);   /* caller gets a handle ... */
    printf("  device_id(handle) = %d  (fields reached only through the API)\n", device_id(h));
    /* A caller in another file cannot write h->open or sizeof(*h): 'struct device'
     * is incomplete to them, so the compiler enforces the encapsulation. */
    return 0;
}
