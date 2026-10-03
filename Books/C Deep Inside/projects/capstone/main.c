/* C Deep Inside for Embedded System Development: Bare-Metal — Capstone
 * A mock sensor node: read a temperature, decide a fan duty, drive a register,
 * log the sample. (runnable demo with SIMULATED registers)
 *
 * This is the whole book in one short program. It is not a driver for any real
 * chip; it is the SHAPE of one, written so you can read each line and say what
 * it costs and does on the target. Every piece points back to a chapter:
 *
 *   Ch4/5  fixed-point temperature in Q8.8 (no float on the MCU)
 *   Ch8    volatile memory-mapped registers reached through a pointer
 *   Ch11   a union overlay of the register block (named fields vs raw words)
 *   Ch13   status flags packed into one register, set/cleared with masks
 *   Ch14   typedef names for the register and fixed-point types
 *   Ch15   const calibration + per-mode gain tables living in "flash"
 *   Ch9    a ring buffer (array + index wrapped with a bitwise mask)
 *
 * As in the Chapter 8 demo, the "registers" are backed by a RAM block so this
 * runs on a host (OnlineGDB, etc.). On silicon you point DEV at the datasheet
 * base address instead; nothing else changes.
 *
 *   gcc -std=c17 -Wall -Wextra -Wconversion main.c -o demo && ./demo
 */
#include <stdio.h>
#include <stdint.h>

/* ---- types (Ch14) ---------------------------------------------------- */
typedef int32_t fixed_t;        /* Q8.8 signed fixed-point: 1.0 == 256     */
typedef uint32_t reg_t;         /* a 32-bit hardware register              */

#define Q           8                      /* 8 fractional bits            */
#define TO_Q(whole) ((fixed_t)((whole) << Q))

/* ---- register block (Ch8 + Ch11) ------------------------------------- */
/* A union overlays named registers on the raw word array at the same
 * addresses: DEV->reg.status IS DEV->raw[0]. 'volatile' means every access
 * is a real bus access the compiler may not fold away or reorder. */
typedef union {
    volatile reg_t raw[4];
    struct {
        volatile reg_t status;   /* +0x00  flags (see ST_* below)         */
        volatile reg_t temp_adc; /* +0x04  raw 12-bit ADC counts          */
        volatile reg_t fan_pwm;  /* +0x08  fan duty 0..255                */
        volatile reg_t ctrl;     /* +0x0C  (unused here; shows the layout)*/
    } reg;
} periph_t;

static periph_t sim_device;                 /* stands in for silicon       */
static periph_t *const DEV = &sim_device;    /* on hardware: (periph_t*)0x40010000 */

/* ---- status flags, one bit each (Ch13) ------------------------------- */
#define ST_FAN_ON    (1u << 0)
#define ST_OVERHEAT  (1u << 1)
#define ST_VALID     (1u << 2)

/* ---- calibration + gain tables in read-only "flash" (Ch15) ----------- */
/* temp(Q8.8) = adc * SLOPE_Q - OFFSET_Q.  ~0.0195 C/count, biased by -10 C. */
static const fixed_t SLOPE_Q    = 5;            /* 5/256 C per ADC count     */
static const fixed_t OFFSET_Q   = TO_Q(10);     /* subtract 10.00 C          */
static const fixed_t SETPOINT_Q = TO_Q(25);     /* target 25.00 C            */
static const fixed_t OVERHEAT_Q = TO_Q(60);     /* flag at 60.00 C           */

typedef enum { MODE_ECO = 0, MODE_NORMAL, MODE_TURBO } mode_t;
static const int32_t GAIN_BY_MODE[3] = { 8, 12, 20 };   /* duty per C of error */
static const char *const MODE_NAME[3] = { "ECO", "NORMAL", "TURBO" };

/* ---- ring buffer: last 8 samples (Ch9) ------------------------------- */
#define RING_SIZE 8u
#define RING_MASK (RING_SIZE - 1u)          /* power-of-two size => AND wraps */
static fixed_t ring[RING_SIZE];
static uint32_t ring_head;

static void ring_push(fixed_t v)
{
    ring[ring_head] = v;
    ring_head = (ring_head + 1u) & RING_MASK;   /* no modulo, just a mask     */
}

/* print a Q8.8 value as a signed decimal, e.g. -0.39, 25.00  (no float) */
static void print_q(fixed_t q)
{
    int neg = (q < 0);
    uint32_t a = neg ? (uint32_t)(-q) : (uint32_t)q;
    uint32_t centi = (a * 100u) >> Q;           /* hundredths of a degree     */
    printf("%s%u.%02u", neg ? "-" : "", centi / 100u, centi % 100u);
}

static int clamp_duty(int32_t d)
{
    if (d < 0)   return 0;
    if (d > 255) return 255;
    return (int)d;
}

int main(void)
{
    const mode_t mode = MODE_NORMAL;
    const int32_t gain = GAIN_BY_MODE[mode];

    /* a canned sweep of raw ADC readings (cool -> hot -> cooling) */
    static const uint16_t adc_samples[] = { 1800, 2300, 2900, 3600, 3550, 2600 };
    const size_t n = sizeof adc_samples / sizeof adc_samples[0];

    printf("Capstone: mock sensor node  [mode=%s, gain=%ld duty/C]\n",
           MODE_NAME[mode], (long)gain);
    printf("setpoint="); print_q(SETPOINT_Q);
    printf(" C   overheat>"); print_q(OVERHEAT_Q); printf(" C\n\n");
    printf("  adc   temp(C)   err(C)   fan   flags\n");
    printf("  ----  --------  -------  ----  --------------------\n");

    for (size_t i = 0; i < n; ++i) {
        uint16_t adc = adc_samples[i];
        DEV->reg.temp_adc = adc;                    /* "write" the ADC result */

        /* fixed-point conversion and proportional control (Ch4/5) */
        fixed_t temp_q = (fixed_t)adc * SLOPE_Q - OFFSET_Q;
        fixed_t err_q  = temp_q - SETPOINT_Q;
        int32_t duty   = ((int32_t)err_q * gain) >> Q;   /* back to 0..255     */
        int fan        = clamp_duty(duty);
        DEV->reg.fan_pwm = (reg_t)fan;

        /* update status flags with masks (Ch13) */
        reg_t st = ST_VALID;
        if (fan > 0)              st |= ST_FAN_ON;
        if (temp_q > OVERHEAT_Q)  st |= ST_OVERHEAT;
        DEV->reg.status = st;

        ring_push(temp_q);

        printf("  %4u  ", adc);
        print_q(temp_q); printf("    ");
        print_q(err_q);  printf("   %3d   ", fan);
        printf("%s%s%s\n",
               (st & ST_VALID)    ? "VALID "    : "",
               (st & ST_FAN_ON)   ? "FAN_ON "   : "",
               (st & ST_OVERHEAT) ? "OVERHEAT"  : "");
    }

    /* read the register block back through the RAW union view (Ch11) */
    printf("\nregister block (raw words, same addresses as the named fields):\n");
    for (size_t i = 0; i < 4; ++i)
        printf("  raw[%zu] = 0x%08X\n", i, DEV->raw[i]);

    /* drain the ring buffer newest-first (Ch9) */
    printf("\nlast %u samples (newest first): ", (unsigned)RING_SIZE);
    for (uint32_t k = 0; k < RING_SIZE; ++k) {
        uint32_t idx = (ring_head - 1u - k) & RING_MASK;
        print_q(ring[idx]); printf(" ");
    }
    printf("\n");
    return 0;
}
