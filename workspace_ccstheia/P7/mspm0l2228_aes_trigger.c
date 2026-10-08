/*
 * mspm0l2228_aes_trigger.c
 * ------------------------------------------------------------------
 * SCA victim firmware for the TI MSPM0L2228 AES-ADV hardware engine.
 *
 * Behaviour: holds a SECRET AES-128 key compiled in below, waits for a
 * 16-byte plaintext over UART, raises a GPIO trigger tightly around a
 * single AES-128 ECB encryption on the hardware accelerator, then
 * returns the 16-byte ciphertext. The key is never sent off-chip.
 *
 * Host protocol (raw bytes, no SimpleSerial framing):
 *   host -> 'p' + 16 plaintext bytes
 *   dev  -> 'r' + 16 ciphertext bytes
 *
 * ------------------------------------------------------------------
 * SETUP IN SysConfig (generates ti_msp_dl_config.h / SYSCFG_DL_init):
 *   - Device: MSPM0L2228, run CPU at a fixed clock (e.g. 32 MHz MCLK).
 *   - AESADV: enable the peripheral.
 *   - UART:   one instance named UART_0, 115200 8N1, TX+RX pins routed
 *             to the pins you wire to the Husky target serial (or a
 *             USB-UART). Baud must match the capture notebook.
 *   - GPIO:   one output named GPIO_TRIG with pin GPIO_TRIG_PIN on
 *             port GPIO_TRIG_PORT, wired to the Husky trigger input
 *             (CW308 gpio4 / tio4). Idle low.
 * Then place this file as the project's main and build with the
 * MSPM0 SDK (CCS/gcc). Function NAMES here are from the SDK AESADV
 * driver; if your SDK version differs on a signature, the compiler
 * will point at the exact line.
 * ------------------------------------------------------------------
 */

#include "ti_msp_dl_config.h"
#include <string.h>
#include <stdint.h>

/* --- Map SysConfig-generated labels (from ti_msp_dl_config.h) ---
 * This build generated: UART_1_INST (UART0 on PA10=TX / PA11=RX, 115200 8N1),
 * GPIO_TRIG_PORT, GPIO_TRIG_PIN_0_PIN (trigger on PA0 / package pin 1).
 * If you rename the SysConfig instances, edit only these. */
#define UART_INST   UART_1_INST
#define TRIG_PORT   GPIO_TRIG_PORT
#define TRIG_PIN    GPIO_TRIG_PIN_0_PIN

/* ---- SECRET KEY (attacker does not know this; edit to your value) ---- */
static const uint8_t SECRET_KEY[16] = {
    0x91, 0xD4, 0x3A, 0x7F, 0xC2, 0x18, 0xE6, 0x55,
    0x0B, 0xA9, 0xF3, 0x6C, 0x42, 0xDE, 0x87, 0x19
};

/* ---- trigger helpers ---- */
static inline void trigger_high(void) { DL_GPIO_setPins(TRIG_PORT,  TRIG_PIN); }
static inline void trigger_low (void) { DL_GPIO_clearPins(TRIG_PORT, TRIG_PIN); }

/* ---- blocking UART byte I/O ---- */
static uint8_t uart_get(void) { return DL_UART_Main_receiveDataBlocking(UART_INST); }
static void    uart_put(uint8_t b) { DL_UART_Main_transmitDataBlocking(UART_INST, b); }

static void uart_get_n(uint8_t *buf, int n) { for (int i = 0; i < n; i++) buf[i] = uart_get(); }
static void uart_put_n(const uint8_t *buf, int n) { for (int i = 0; i < n; i++) uart_put(buf[i]); }

/* ---- one hardware AES-128 ECB block; trigger brackets the core op ----
 *
 * Follows the TI MSPM0 SDK AES-ADV sequence exactly (see the SDK's
 * aesadv_ecb_*_encrypt driverlib examples):
 *   1. wait for the input context to be writeable,
 *   2. load the key, THEN commit the ECB/ENCRYPT context (initECB),
 *   3. wait for input-ready, then loadInputData -- loading a full block
 *      is what STARTS the core (no forceInputDataAvailable needed; that
 *      is only for partial/streaming blocks),
 *   4. poll isOutputReady, then read the ciphertext.
 * Re-committing key+context every call (all before trigger_high) keeps the
 * engine in an identical state for every capture, which matters for CPA. */
static void aes_encrypt_block(const uint8_t in[16], uint8_t out[16])
{
    while (!DL_AESADV_isInputContextWriteable(AESADV)) { }
    DL_AESADV_setKey(AESADV, (uint8_t *)SECRET_KEY, DL_AESADV_KEY_SIZE_128_BIT);
    SYSCFG_DL_AESADV_init();                      /* initECB: apply ECB/ENCRYPT context */

    while (!DL_AESADV_isInputReady(AESADV)) { }

    trigger_high();
    DL_AESADV_loadInputData(AESADV, (uint8_t *)in);   /* loading the block starts the core */
    while (!DL_AESADV_isOutputReady(AESADV)) { }       /* wait for completion */
    trigger_low();

    DL_AESADV_readOutputData(AESADV, out);
}

int main(void)
{
    SYSCFG_DL_init();          /* SysConfig: clocks, UART_1, GPIO_TRIG, AESADV */
    trigger_low();

    uint8_t pt[16], ct[16];

    for (;;) {
        uint8_t cmd = uart_get();
        if (cmd != 'p') continue;      /* resync on the command byte */

        uart_get_n(pt, 16);
        aes_encrypt_block(pt, ct);

        uart_put('r');
        uart_put_n(ct, 16);
    }
}