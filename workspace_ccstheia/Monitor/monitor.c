#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdbool.h>

#define DELAY         (16000000)
#define APP_ADDRESS   (0x00020000UL)
#define SRAM_START    (0x20200000UL)
#define SRAM_END      (0x20208000UL)
#define APP_FLASH_END (0x00040000UL)

volatile uint32_t monitor_ticks = 0;

typedef void (*app_entry_t)(void);

// variables for random state preservation.
static uint32_t randomState;
static volatile uint32_t noiseSink = 0;

// check if application is in correct memory location
static int validate_application(void)
{
    uint32_t app_stack = *(volatile uint32_t *)APP_ADDRESS;
    uint32_t app_reset = *(volatile uint32_t *)(APP_ADDRESS + 4U);

    uint32_t reset_address = app_reset & ~1UL;

    int stack_valid =
        (app_stack > SRAM_START) &&
        (app_stack <= SRAM_END) &&
        ((app_stack & 7U) == 0U);

    int reset_valid =
        ((app_reset & 1U) != 0U) &&
        (reset_address >= APP_ADDRESS) &&
        (reset_address < APP_FLASH_END);

    return stack_valid && reset_valid;
}


// start application routine
__attribute__((naked, noreturn))
static void start_application(uint32_t app_stack,
                              uint32_t app_reset)
{
    __asm volatile(
        "msr msp, r0 \n"
        "cpsie i     \n"
        "bx r1       \n"
    );
}

// Fast pseudorandom number generator for testing
uint32_t fastRandom(void)
{
    uint32_t x = randomState;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    randomState = x;
    return x;
}

// Monitor interrupt routine
void SysTick_Handler(void)
{
    // 1. Randomize how long the monitor runs
    uint32_t duration = 20u + (fastRandom() % 100u);

    // 2. Perform dummy computations
    uint32_t value = noiseSink;

    for (uint32_t i = 0; i < duration; i++) {
        value ^= value << 7;
        value += 0x9E3779B9u + i;
        value ^= value >> 9;
    }

    noiseSink = value;

    // 3. Randomize time until next interrupt
    // Approximately 0.5-1.0 ms at 32 MHz
    uint32_t interval = 16000u + (fastRandom() % 16001u);

    // 4. Schedule next interrupt
    SysTick->LOAD = interval - 1u;
    SysTick->VAL = 0u;

   
    // DL_UART_Main_transmitDataBlocking(UART_0_INST, 'I');
    // DL_UART_Main_transmitDataBlocking(UART_0_INST, '\r');
    // DL_UART_Main_transmitDataBlocking(UART_0_INST, '\n');
}



// get random seed
bool getRandomNumber(uint32_t *random)
{
    // Check output pointer
    if (random == NULL) {
        return false;
    }

    // Wait for a random value, with a timeout
    uint32_t timeout = 1000000u;

    while (!DL_TRNG_isCaptureReady(TRNG)) {

        if (DL_TRNG_isHealthTestFail(TRNG) ||
            DL_TRNG_isCommandFail(TRNG)) {
            return false;
        }

        if (--timeout == 0u) {
            return false;
        }
    }

    // Reject reported TRNG health failures
    if (DL_TRNG_isHealthTestFail(TRNG)) {
        return false;
    }

    // Retrieve the 32-bit random number
    *random = DL_TRNG_getCapture(TRNG);

    // Clear the capture-ready flag
    DL_TRNG_clearInterruptStatus(
        TRNG,
        DL_TRNG_INTERRUPT_CAPTURE_RDY_EVENT
    );

    return true;
}


bool initializeRandom(void)
{
    uint32_t seed;

    if (!getRandomNumber(&seed)) {
        return false;
    }

    // Xorshift requires a nonzero state
    randomState = (seed != 0u) ? seed : 1u;

    return true;
}

// Initialize the interrupt handler
void initializeSysTick(void)
{
    DL_SYSTICK_init(3200000u);

    DL_SYSTICK_enableInterrupt();

    DL_SYSTICK_enable();
}

int main(void)
{
    SYSCFG_DL_init();

    DL_GPIO_clearPins(GPIO_LEDS_USER_LED_1_PORT,
                      GPIO_LEDS_USER_LED_1_PIN);
    // Enable and configure TRNG
    DL_TRNG_enablePower(TRNG);

    uint32_t randomValue;

    DL_UART_Main_transmitDataBlocking(UART_0_INST, 'A');
    DL_UART_Main_transmitDataBlocking(UART_0_INST, '\r');
    DL_UART_Main_transmitDataBlocking(UART_0_INST, '\n');

    if (initializeRandom()) {

        uint32_t app_stack =
        *(volatile uint32_t *)APP_ADDRESS;

        uint32_t app_reset =
            *(volatile uint32_t *)(APP_ADDRESS + 4U);

        while (!validate_application())
        {
            delay_cycles(DELAY);
            DL_UART_Main_transmitDataBlocking(UART_0_INST, 'A');
            DL_UART_Main_transmitDataBlocking(UART_0_INST, '\r');
            DL_UART_Main_transmitDataBlocking(UART_0_INST, '\n');
        }

        initializeSysTick();
        start_application(app_stack, app_reset);

        
    } else {
        
        DL_UART_Main_transmitDataBlocking(UART_0_INST, 'E');
        DL_UART_Main_transmitDataBlocking(UART_0_INST, '\r');
        DL_UART_Main_transmitDataBlocking(UART_0_INST, '\n');
    }

    while (1) {

        DL_UART_Main_transmitDataBlocking(UART_0_INST, 'E');
        DL_UART_Main_transmitDataBlocking(UART_0_INST, '\r');
        DL_UART_Main_transmitDataBlocking(UART_0_INST, '\n');
    }


}