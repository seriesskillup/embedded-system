/* C Deep Inside for Embedded System Development: Bare-Metal — Chapter 8: Pointers, volatile, and memory-mapped I/O
 * (runnable demo with SIMULATED registers)
 *
 * On a real microcontroller a peripheral register lives at a fixed address on
 * the chip, for example the UART data register at 0x40011004. That address is
 * NOT valid memory on a PC, so to keep this example runnable the "registers"
 * are backed by a small RAM bank, sim_regs[]. The reads, writes, volatile
 * behaviour, and read-modify-write hazard all behave the same; only the backing
 * store differs. On silicon you would point the same pointer at the datasheet
 * address instead of into sim_regs[].
 *
 * Covers: address vs content; arithmetic scaled by type; *p++ vs (*p)++;
 * const/volatile placement; a volatile register write; the read-modify-write
 * lost-update hazard and the dedicated set-register fix; a function-pointer
 * dispatch table (the shape of a vector table).
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>

/* ---- Simulated peripheral space (stands in for real registers) ---- */
static volatile uint32_t sim_regs[16];
/* On hardware these would be: (volatile uint32_t *)0x4001100x. Here they are
 * offsets into sim_regs[]. Each is 'volatile': every access is a real access. */
#define UART_DR   (sim_regs[1])          /* data register: a write "transmits"  */
#define PORT      (sim_regs[2])          /* a read-write output port            */
#define PORT_SET  (sim_regs[3])          /* a dedicated set register (like BSRR) */

static void mirror_set_register(void)
{
    /* A real BSRR-style set register sets the bits written to it, atomically,
     * in the port. We mirror that here so the effect is observable. */
    PORT |= PORT_SET;
    PORT_SET = 0;
}

/* dispatch table entries (the shape of a vector table) */
static void cmd_start(void) { puts("    -> cmd_start()"); }
static void cmd_stop(void)  { puts("    -> cmd_stop()");  }

int main(void)
{
    puts("== address vs content ==");
    int x = 42;
    int *p = &x;                 /* p holds the ADDRESS of x */
    printf("  &x = %p   *p = %d\n", (void *)p, *p);   /* the address, then the content */

    puts("\n== arithmetic is scaled by the pointed-to type ==");
    uint8_t  bytes[4] = { 0 };
    uint32_t words[4] = { 0 };
    uint8_t  *bp = bytes;
    uint32_t *wp = words;
    printf("  bp+1 advances %ld byte(s)\n",  (long)((char *)(bp + 1) - (char *)bp));
    printf("  wp+1 advances %ld byte(s)\n",  (long)((char *)(wp + 1) - (char *)wp));

    puts("\n== *p++ vs (*p)++ vs ++*p (each starts from cell = 10) ==");
    int cells[2] = { 10, 10 };
    int *q = cells;
    int e1 = *q++;               /* read *q (10), THEN advance the pointer */
    printf("  *q++  yielded %d, pointer moved, cell still %d\n", e1, cells[0]);

    cells[0] = 10; q = cells;
    int e2 = (*q)++;             /* read *q (10), THEN increment the VALUE */
    printf("  (*q)++ yielded %d, cell now %d\n", e2, cells[0]);

    cells[0] = 10; q = cells;
    int e3 = ++*q;               /* increment the VALUE first, then use it */
    printf("  ++*q  yielded %d, cell now %d\n", e3, cells[0]);

    puts("\n== const / volatile placement (the everyday register pointer) ==");
    /* volatile uint32_t *const : fixed address, every access real. */
    volatile uint32_t *const dr = &UART_DR;
    *dr = (uint32_t)'A';         /* this "write" transmits a byte on real HW    */
    printf("  wrote 'A' to UART_DR; reads back 0x%02X ('%c')\n",
           (unsigned)*dr, (char)*dr);

    puts("\n== read-modify-write lost-update hazard ==");
    PORT = 0x20u;                /* bit 5 set */
    /* main does PORT |= (1u<<3) as load, modify, store. We split it and inject
     * an "interrupt" that sets bit 7 in the middle, to make the loss visible. */
    uint32_t tmp = PORT;         /* load  -> 0x20 */
    PORT |= (1u << 7);           /* <-- pretend an ISR sets bit 7 here (PORT = 0xA0) */
    tmp |= (1u << 3);            /* modify the STALE copy -> 0x28 */
    PORT = tmp;                  /* store -> 0x28, erasing the ISR's bit 7 */
    printf("  after |= RMW: PORT = 0x%02X  (bit 7 lost; expected 0xA8)\n", (unsigned)PORT);

    /* The fix: a dedicated set register sets the bit without a read. */
    PORT = 0x20u;
    PORT |= (1u << 7);           /* the same "ISR" sets bit 7 (PORT = 0xA0) */
    PORT_SET = (1u << 3);        /* ask hardware to set bit 3, no read of PORT */
    mirror_set_register();
    printf("  with set register: PORT = 0x%02X  (bit 7 kept)\n", (unsigned)PORT);

    puts("\n== function-pointer dispatch table (vector-table shape) ==");
    typedef void (*cmd_fn)(void);
    static const cmd_fn dispatch[] = { cmd_start, cmd_stop };
    for (size_t i = 0; i < sizeof dispatch / sizeof dispatch[0]; i++)
        if (dispatch[i])                 /* null-check before calling */
            dispatch[i]();               /* bounds already guaranteed by the loop */

    return 0;
}
