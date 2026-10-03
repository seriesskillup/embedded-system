/* C Deep Inside for Embedded System Development: Bare-Metal — Dynamic Memory Allocation (Chapter 16)
 *
 * Covers: the malloc/free discipline (check NULL, size from the object, NULL out
 * after free); the realloc-needs-a-temporary rule; and a fixed-block pool with
 * O(1), fragmentation-free alloc/free and a high-water reading. The four heap
 * bugs and the stack/heap collision are undefined/unsafe to run, so they are
 * described, not executed.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* ---- a fixed-block pool: what most embedded "dynamic" needs really want ---- */
#define POOL_N   4
#define BLOCK_SZ 32
static union block { union block *next; unsigned char data[BLOCK_SZ]; } pool[POOL_N];
static union block *free_list;
static int in_use, in_use_max;         /* high-water: most blocks used at once */

static void pool_init(void) {
    free_list = NULL;
    for (size_t i = 0; i < POOL_N; i++) { pool[i].next = free_list; free_list = &pool[i]; }
    in_use = in_use_max = 0;
}
static void *pool_alloc(void) {         /* O(1): unlink the head */
    union block *b = free_list;
    if (!b) return NULL;                /* bounded failure: no UB, no fault */
    free_list = b->next;
    if (++in_use > in_use_max) in_use_max = in_use;
    return b;
}
static void pool_free(void *p) {        /* O(1): push back on the head */
    if (!p) return;
    union block *b = p; b->next = free_list; free_list = b; in_use--;
}

int main(void)
{
    puts("== Ch16: the malloc/free discipline ==");
    size_t count = 3;
    int *p = malloc(count * sizeof *p); /* size FROM THE OBJECT, not a literal */
    if (!p) return 1;                   /* ALWAYS check for NULL */
    for (size_t i = 0; i < count; i++) p[i] = (int)i * 10;
    printf("  malloc'd %zu ints: %d %d %d\n", count, p[0], p[1], p[2]);
    free(p);
    p = NULL;                           /* defuse double-free / use-after-free */
    printf("  freed and set p = NULL\n");

    puts("\n== Ch16: realloc needs a temporary ==");
    int *buf = malloc(2 * sizeof *buf);
    void *tmp = realloc(buf, 4 * sizeof *buf);   /* hold the result first */
    if (!tmp) { free(buf); return 1; }           /* on failure buf is still valid */
    buf = tmp;
    printf("  realloc grew the block safely (no leak on failure)\n");
    free(buf);

    puts("\n== Ch16: a fixed-block pool (deterministic, no fragmentation) ==");
    pool_init();
    void *a = pool_alloc(), *b = pool_alloc(), *c = pool_alloc();
    printf("  allocated 3 of %d blocks; in_use = %d\n", POOL_N, in_use);
    pool_free(b);
    void *d = pool_alloc(), *e = pool_alloc();   /* one reused, one fresh */
    printf("  after free+2 allocs: in_use = %d, exhausted alloc returns %s\n",
           in_use, pool_alloc() ? "a block" : "NULL");
    printf("  high-water (max blocks used at once) = %d  -> size the pool from this\n", in_use_max);
    (void)a; (void)c; (void)d; (void)e;

    /* NOT run (undefined / unsafe): leak, double free, use-after-free, buffer
     * overrun, and a stack/heap collision. On a no-MMU MCU these corrupt memory
     * silently; the pool above avoids them by construction. */
    return 0;
}
