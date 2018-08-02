#include <stdbool.h>
#include <stdint.h>
// #include "nrf_delay.h"
#include "boards.h"
#define NRF_LOG_MODULE_NAME
#include "app_uart.h"
#include "bsp.h"
#include "app_error.h"

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

#include "unity.h"
#include <stdio.h>
#include <setjmp.h>
#include "jumper.h"

void uart_error_handle(app_uart_evt_t * p_event)
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

extern void setUp(void);
extern void tearDown(void);
extern void test_CallTheButtonCallback(void);


void resetTest(void);
void resetTest(void)
{
    tearDown();
    setUp();
}

int main(void)
{
    uint32_t err_code;
    const app_uart_comm_params_t comm_params =
        {
            RX_PIN_NUMBER,
            TX_PIN_NUMBER,
            RTS_PIN_NUMBER,
            CTS_PIN_NUMBER,
            APP_UART_FLOW_CONTROL_ENABLED,
            false,
            UART_BAUDRATE_BAUDRATE_Baud115200
        };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);

    APP_ERROR_CHECK(err_code);
    bsp_board_leds_init();

    UnityBegin("test/TestProductionCode.c");
    
    RUN_TEST(test_CallTheButtonCallback);

    int unity_code = UnityEnd();
    jumper_sudo_exit_with_exit_code(unity_code);
}
