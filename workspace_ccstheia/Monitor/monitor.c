#include "ti_msp_dl_config.h"
#include <stdint.h>

#define DELAY         (16000000)
#define APP_ADDRESS   (0x00020000UL)
#define SRAM_START    (0x20200000UL)
#define SRAM_END      (0x20208000UL)
#define APP_FLASH_END (0x00040000UL)

uint32_t APP_FOUND = 0;
volatile uint32_t monitor_ticks = 0;

typedef void (*app_entry_t)(void);


/*This function will allow the B program to execute,
by first checking it's instance*/
static void start_application(void)
{
    /* First entry in B's vector table = initial stack pointer */
    uint32_t app_stack =
        *((volatile uint32_t *) APP_ADDRESS);

    /* Second entry = address of B's Reset_Handler */
    uint32_t app_reset =
        *((volatile uint32_t *) (APP_ADDRESS + 4U));


    /* Do not allow an interrupt in the middle of the handoff */
    __disable_irq();

    /* Give B the stack pointer it was linked/started with */
    __set_MSP(app_stack);

    /* Jump to B's Reset_Handler */
    ((app_entry_t) app_reset)();

    /* B should never return here */
    while (1) {
        delay_cycles(DELAY);
    }
}

static void validate_application(void)
{

        /* Read Application B's vector table */
        uint32_t app_stack =
            *((volatile uint32_t *) APP_ADDRESS);

        uint32_t app_reset =
            *((volatile uint32_t *) (APP_ADDRESS + 4U));

        /* Ignore Thumb bit when checking reset-handler address */
        uint32_t reset_address = app_reset & ~1UL;

        /* Check that B's stack pointer points into SRAM */
        int SRAM_FOUND =
            (app_stack > SRAM_START) &&
            (app_stack <= SRAM_END) &&
            ((app_stack & 0x7U) == 0U);

        /* Check that B's Reset_Handler points into B's flash */
        int RESET_HANDLER_FLASH =
            (reset_address >= APP_ADDRESS) &&
            (reset_address < APP_FLASH_END);

        /* Application looks valid */
        if ((SRAM_FOUND == 1) &&
            (RESET_HANDLER_FLASH == 1)) {
            APP_FOUND = 1;
        }
        
}

void SysTick_Handler(void)
{
    monitor_ticks++;

    DL_GPIO_togglePins(GPIO_LEDS_USER_LED_1_PORT,
                       GPIO_LEDS_USER_LED_1_PIN);
}


int main(void)
{
    SYSCFG_DL_init();


    // Example: 0.5 seconds at a 32 MHz SysTick clock
    SysTick->LOAD = 16000000U - 1U;
    SysTick->VAL = 0U;

    // Enable SysTick, its interrupt, and CPU clock source
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;

    __enable_irq();

    while (1)
    {
        // CPU executes normally.
        // SysTick interrupts occur independently.
    }


    // /* Make the initial state obvious. */
    // DL_GPIO_clearPins(GPIO_LEDS_USER_LED_1_PORT,
    //                   GPIO_LEDS_USER_LED_1_PIN);


    // /* Monitor indicator: LED 1 ON. */
    // DL_GPIO_setPins(GPIO_LEDS_USER_LED_1_PORT,
    //                 GPIO_LEDS_USER_LED_1_PIN);

    // /* Approximately 2 seconds: 4 × ~0.5 s. */
    // delay_cycles(DELAY);
    // delay_cycles(DELAY);
    // delay_cycles(DELAY); 
    // delay_cycles(DELAY);

    // /* Monitor is finished. Turn LED 1 OFF. */
    // DL_GPIO_clearPins(GPIO_LEDS_USER_LED_1_PORT,
    //                   GPIO_LEDS_USER_LED_1_PIN);
    
    // /* Transfer execution to the application's Reset_Handler. */
    // validate_application();
    // if (APP_FOUND == 1){
    //     start_application();
    // }

    
    // /* We should never get here. */
    // while (1) {
    //     delay_cycles(DELAY);
    // }

}