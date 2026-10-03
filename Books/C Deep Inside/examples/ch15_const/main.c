/* C Deep Inside for Embedded System Development: Bare-Metal — const & Read-only Data (Chapter 15)
 *
 * Covers: the four const-pointer forms and which part each locks;
 * const-correctness across an API (a reader takes const, so a flash table needs
 * no cast); a const table living in read-only storage; and the const volatile
 * status register (read-only to you, changed by hardware), shown with a
 * simulated register.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>

/* const-correct: promises not to write through data, so a const array passes
 * without a cast. */
static uint32_t checksum(const uint8_t *data, size_t n)
{
    uint32_t s = 0;
    for (size_t i = 0; i < n; i++) s += data[i];
    return s;
}

/* const table: lives in read-only storage (.rodata / flash on an MCU). */
static const uint8_t table[4] = { 1, 2, 3, 4 };

/* simulated const volatile status register (hardware writes it, you only read) */
static volatile uint32_t sim_status = 0x1u;

int main(void)
{
    puts("== Ch15: the four const-pointer forms ==");
    int a = 1, b = 2;
    const int *pcd = &a;           /* pointer to const int: may repoint, may NOT write *pcd */
    int *const cpd = &a;           /* const pointer to int: may write *cpd, may NOT repoint */
    pcd = &b;                      /* OK: repoint */
    *cpd = 5;                      /* OK: write through */
    printf("  pcd repointed to b (=%d); *cpd written -> a is now %d\n", *pcd, a);
    /* These two lines would NOT compile (uncomment to see the error):
     *   *pcd = 9;   // error: assignment of read-only location
     *   cpd = &b;   // error: assignment of read-only variable 'cpd'  */

    puts("\n== Ch15: const-correctness lets a const table pass without a cast ==");
    printf("  checksum(table) = %u  (flash data handed straight in)\n",
           checksum(table, sizeof table));

    puts("\n== Ch15: const volatile status register ==");
    /* const volatile: 'volatile' forces a real read each time, 'const' stops your
     * code writing it. Here sim_status stands in for a hardware register. */
    const volatile uint32_t *status = &sim_status;
    printf("  status bit0 (ready) = %u  (fresh read; a write would not compile)\n",
           (unsigned)(*status & 1u));
    return 0;
}
