
#include "hal.h"
#include "sam4s.h"
#include <stdint.h>

// Simple blocking delay
void delay(uint32_t cycles)
{
    for (volatile uint32_t i = 0; i < cycles; i++) {
        __NOP();
    }
}

int main(void)
{
    platform_init();
    init_uart();

    while (1) {
        // Print customer identifier
        //putch('C');

        // Delay before printing again
        delay(1000000);
    }
}
