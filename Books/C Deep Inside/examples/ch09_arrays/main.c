/* Deep C for Bare-Metal — Chapter 9: Arrays (runnable demo)
 *
 * Covers the chapter's key points, each printed so you can see it:
 *   - array-to-pointer decay, and sizeof(array) vs sizeof(pointer)
 *   - arr vs &arr (same address, different type and pointer arithmetic)
 *   - a[i] vs p[i] (direct element vs load-through-pointer, same result)
 *   - the ARRAY_SIZE idiom, and why it breaks on a pointer
 *   - a 2-D array and its decay to int (*)[N]
 *   - the half-open range [0, n): valid indices are 0 .. n-1
 *
 * Runs as-is on any hosted GCC (OnlineGDB, etc.):
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stddef.h>

/* Count elements of a REAL ARRAY only. On a pointer this gives nonsense,
 * because sizeof(pointer) / sizeof(element) is not the element count. */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void show_sizes(int a[4])   /* NOTE: 'a' here is a POINTER, not an array */
{
    /* Inside a function, an array parameter has already decayed to a pointer,
     * so sizeof(a) is the size of a pointer, not of the 4-int array. This is the
     * classic trap: ARRAY_SIZE(a) here would be wrong.
     *
     * The next line INTENTIONALLY triggers a compiler warning:
     *   "'sizeof' on array function parameter 'a' will return size of 'int *'"
     *   (-Wsizeof-array-argument). That is the compiler catching this exact trap.
     * The program still runs; the warning is the lesson. */
    printf("  inside function: sizeof(a) = %zu (a pointer, not the array)\n", sizeof a);
}

int main(void)
{
    int arr[4] = { 10, 20, 30, 40 };
    int *p = arr;                 /* decay: arr becomes &arr[0], an int* */

    puts("== decay and sizeof ==");
    printf("  sizeof(arr) = %zu bytes  (whole array: 4 ints)\n", sizeof arr);
    printf("  sizeof(p)   = %zu bytes  (just a pointer)\n", sizeof p);
    printf("  ARRAY_SIZE(arr) = %zu elements\n", ARRAY_SIZE(arr));
    show_sizes(arr);

    puts("\n== arr vs &arr (same address, different type) ==");
    printf("  arr   = %p\n", (void *)arr);
    printf("  &arr  = %p  (same address)\n", (void *)&arr);
    /* But the TYPES differ, so pointer arithmetic differs:
     *   arr + 1  advances by one int   (4 bytes)
     *   &arr + 1 advances by the whole array (16 bytes) */
    printf("  arr + 1  advances %ld bytes (one int)\n",
           (long)((char *)(arr + 1) - (char *)arr));
    printf("  &arr + 1 advances %ld bytes (the whole array)\n",
           (long)((char *)(&arr + 1) - (char *)&arr));

    puts("\n== a[i] vs p[i] (same result) ==");
    printf("  arr[2] = %d, p[2] = %d  (both 30)\n", arr[2], p[2]);
    /* a[i] is *(a + i): the compiler knows arr's address directly.
     * p[i] is *(p + i): it first loads the pointer p, then indexes. Same value. */

    puts("\n== the half-open range [0, n) ==");
    /* Valid indices are 0 .. n-1. Index n (== 4 here) is one past the end:
     * you may form &arr[4] as a loop limit, but must never read or write it. */
    for (size_t i = 0; i < ARRAY_SIZE(arr); i++)   /* i in [0, 4): 0,1,2,3 */
        printf("  arr[%zu] = %d\n", i, arr[i]);

    puts("\n== 2-D array decays to int (*)[3] ==");
    int grid[2][3] = { { 1, 2, 3 }, { 4, 5, 6 } };
    /* 'grid' decays to a pointer to its first ROW: int (*)[3], not int**. */
    int (*row)[3] = grid;
    printf("  grid[1][2] = %d\n", grid[1][2]);
    printf("  row[1][2]  = %d  (row is int (*)[3])\n", row[1][2]);
    printf("  sizeof(grid)    = %zu (2 rows x 3 ints)\n", sizeof grid);
    printf("  sizeof(grid[0]) = %zu (one row of 3 ints)\n", sizeof grid[0]);

    /* TRY THIS on your web IDE to see a real diagnostic:
     *   int bad = arr[4];            // read one past the end (undefined behaviour)
     * Compiled with -Wall -Wextra the compiler often warns:
     *   "array subscript 4 is above array bounds of 'int[4]'".
     * With -fsanitize=address it reports a stack-buffer-overflow at run time. */

    return 0;
}
