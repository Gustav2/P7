#include "ti_msp_dl_config.h"
#include <stdint.h>

#define DELAY (16000000U)

int main(void)
{
    SYSCFG_DL_init();

    while (1)
    {
        DL_GPIO_togglePins(GPIO_LEDS_USER_LED_2_PORT,
                           GPIO_LEDS_USER_LED_2_PIN);

        delay_cycles(DELAY);


        DL_UART_Main_transmitDataBlocking(UART_0_INST, 'B');
        DL_UART_Main_transmitDataBlocking(UART_0_INST, '\r');
        DL_UART_Main_transmitDataBlocking(UART_0_INST, '\n');
    }
}