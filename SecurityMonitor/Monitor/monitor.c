
#include "hal.h"
#include "sam4s.h"
#include <stdint.h>
#include <stdio.h>

#define CUSTOMER_BASE       0x00410000u
#define CUSTOMER_FLASH_END  0x00420000u

// Must match customer.ld
#define CUSTOMER_RAM_START  0x20008000u
#define CUSTOMER_RAM_END    0x20010000u

// Monitor SysTick interrupt
// ==========================================
// Configuration
// ==========================================

#define MIN_INTERVAL_MS  20u
#define MAX_INTERVAL_MS  50u

#define MIN_DUMMY_WORK   100u
#define MAX_DUMMY_WORK   500u

// Set to 0 when measuring power traces
#define DEBUG_UART       1

// ==========================================
// Monitor state
// ==========================================

static volatile uint32_t interrupt_count = 0;
static volatile uint32_t dummy_state = 0x12345678u;

// Nonzero PRNG seed (deterministic for testing)
static uint32_t random_state = 0xA5B4C3D2u;

static uint32_t ticks_per_ms = 0;

//===================================
// MEASURE DUMMY WORK
//===================================


void init_cycle_counter(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static uint32_t random32(void)
{
    uint32_t x = random_state;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    random_state = x;
    return x;
}

uint32_t measure_dummy_work(uint32_t *iteration_count)
{
    uint32_t start = DWT->CYCCNT;

    uint32_t range =
        MAX_DUMMY_WORK - MIN_DUMMY_WORK + 1u;

    uint32_t n =
        MIN_DUMMY_WORK + (random32() % range);

    uint32_t x = dummy_state;

    for (uint32_t i = 0; i < n; i++) {

        uint32_t r = random32();

        switch (r & 3u) {
        case 0:
            x ^= (r << 7);
            break;

        case 1:
            x = x * 1664525u + r;
            break;

        case 2:
            x = (x << 5) | (x >> 27);
            x ^= r;
            break;

        default:
            x += (r ^ i);
            x ^= x >> 11;
            break;
        }
    }

    dummy_state = x;

    uint32_t cycles = DWT->CYCCNT - start;

    *iteration_count = n;
    return cycles;
}


#define MEASUREMENTS 10000u

void benchmark_dummy(void)
{
    uint64_t total_cycles = 0;
    uint64_t total_iterations = 0;

    for (uint32_t i = 0; i < MEASUREMENTS; i++) {

        uint32_t n;
        uint32_t cycles = measure_dummy_work(&n);

        total_cycles += cycles;
        total_iterations += n;
    }

    double avg_cycles_per_iteration =
        (double)total_cycles / total_iterations;

    double avg_time_us =
        (avg_cycles_per_iteration * 1000000.0)
        / SystemCoreClock;

    // Print or inspect results after the benchmark

    char buffer[100];

    snprintf(buffer, sizeof(buffer),
            "Time: %.3f us, Cycles: %.2f\r\n",
            avg_time_us, avg_cycles_per_iteration);

    for (char *p = buffer; *p; p++) {
        putch(*p);
    }
}



// ==========================================
// Xorshift32 PRNG
// ==========================================



// ==========================================
// Configure next interrupt interval
// ==========================================

static void schedule_next_interrupt(void)
{
    uint32_t range =
        MAX_INTERVAL_MS - MIN_INTERVAL_MS + 1u;

    uint32_t interval_ms =
        MIN_INTERVAL_MS + (random32() % range);

    uint32_t ticks = interval_ms * ticks_per_ms;

    // Reload register stores ticks - 1
    SysTick->LOAD = ticks - 1u;

    // Reset the current counter so the new
    // interval begins immediately
    SysTick->VAL = 0u;
}

// ==========================================
// Randomized dummy computations
// ==========================================

static void dummy_computations(void)
{
    uint32_t range =
        MAX_DUMMY_WORK - MIN_DUMMY_WORK + 1u;

    uint32_t iterations =
        MIN_DUMMY_WORK + (random32() % range);

    uint32_t x = dummy_state;

    for (uint32_t i = 0; i < iterations; i++) {

        uint32_t r = random32();

        // Select one of four computations
        switch (r & 3u) {

        case 0:
            x ^= (r << 7);
            break;

        case 1:
            x = x * 1664525u + r;
            break;

        case 2:
            x = (x << 5) | (x >> 27);
            x ^= r;
            break;

        default:
            x += (r ^ i);
            x ^= x >> 11;
            break;
        }
    }

    // Preserve the result and prevent the
    // dummy work from being optimized away
    dummy_state = x;
}

// ==========================================
// Monitor SysTick interrupt
// ==========================================

void SysTick_Handler(void)
{

    // Choose when the next interrupt occurs
    schedule_next_interrupt();

    // Perform randomized workload
    dummy_computations();

    interrupt_count++;

#if DEBUG_UART
    // Debug only: print once every 16 interrupts
    if ((interrupt_count % 16u) == 0u) {
        //putch('I');
    }
#endif

}

// ==========================================
// Transfer execution to customer
// ==========================================

// Validate Customer vector table
static int validate_customer(uint32_t *stack,
                             uint32_t *reset)
{
    *stack = *(volatile uint32_t *)CUSTOMER_BASE;
    *reset = *(volatile uint32_t *)(CUSTOMER_BASE + 4u);

    // Stack must be in Customer SRAM and 8-byte aligned
    if (*stack < CUSTOMER_RAM_START ||
        *stack > CUSTOMER_RAM_END ||
        (*stack & 7u) != 0u) {
        return 0;
    }

    // Reset handler must be Thumb code in Customer flash
    uint32_t address = *reset & ~1u;

    if ((*reset & 1u) == 0u ||
        address < CUSTOMER_BASE ||
        address >= CUSTOMER_FLASH_END) {
        return 0;
    }

    return 1;
}

// Assembly handoff: r0 = new SP, r1 = reset address
__attribute__((naked, noreturn))
static void branch_to_customer(uint32_t stack,
                               uint32_t reset)
{
    // __asm volatile(
    //     "msr msp, r0 \n"
    //     "cpsie i      \n"
    //     "bx r1        \n"
    // );

      __asm volatile(
        "msr psp, r0     \n"
        "movs r2, #2     \n"
        "msr control, r2 \n"
        "isb             \n"
        "cpsie i         \n"
        "bx r1           \n"
    );


}

// Transfer control to Customer
__attribute__((noreturn))
static void start_customer(void)
{
    uint32_t stack;
    uint32_t reset;

    if (!validate_customer(&stack, &reset)) {
        //putch('E');

        while (1) {
        }
    }

    // Indicate that validation succeeded
    //putch('J');

    // Stop Monitor interrupts for this initial test
    __disable_irq();

    // Use Customer interrupt vector table
    SCB->VTOR = CUSTOMER_BASE;
    __DSB();
    __ISB();

    // Branch directly to Customer reset handler
    branch_to_customer(stack, reset);
}

int main(void)
{
    platform_init();
    init_uart();
    init_cycle_counter();
    //benchmark_dummy();
    //putch('M');

    // Number of SysTick counts per millisecond
    ticks_per_ms = SystemCoreClock / 1000u;

    // Begin with an initial 80 ms interval.
    // Every subsequent interval will be randomized.
    if (SysTick_Config(ticks_per_ms * 80u) != 0) {
        //putch('E');
        while (1) {}
    }

    __enable_irq();

     // Observe first five interrupts
    // uint32_t reported = 0;

    // while (reported < 5u) {
    //     if (interrupt_count > reported) {
    //         reported++;
    //         //putch('A');
    //     }
    // }

    // transfer to customer
    start_customer();
}
