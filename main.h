#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "bsp.h"
#include "app_timer.h"
#include "app_error.h"
// #include "nrf_drv_wdt.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_uart.h"
#include "boards.h"
#define NRF_LOG_MODULE_NAME

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    bsp_board_leds_off();
    while (1)
        ;
}

void bsp_event_callback(bsp_event_t event)
{
    switch (event)
    {
    case BSP_EVENT_KEY_0:
        // nrf_drv_wdt_channel_feed(m_channel_id);
        bsp_board_led_on(0);
        break;

    default:
        //Do nothing.
        break;
    }
}

void uart_error_handle(app_uart_evt_t *p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}