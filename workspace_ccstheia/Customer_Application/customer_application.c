#include "ti_msp_dl_config.h"
#include <stdint.h>


#define DELAY         (16000000)


int main(void)
{
    SYSCFG_DL_init();

    DL_GPIO_clearPins(GPIO_LEDS_USER_LED_2_PORT,
                      GPIO_LEDS_USER_LED_2_PIN);


    DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT,
                    GPIO_LEDS_USER_LED_2_PIN);

    delay_cycles(DELAY);
    delay_cycles(DELAY);
    delay_cycles(DELAY); 
    delay_cycles(DELAY);

   
    /* We should never get here. */
    while (1) {
      
    }

    
}