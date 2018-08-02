#include "unity.h"
#include "jumper.h"
#include "main.h"


/*************/
/* UNIT TEST */
/*************/

void setUp(void){}

void tearDown(void){}

void test_1_equal_1(void) {
    int expected = 1;
    int actual = 3;
    TEST_ASSERT_EQUAL(expected, actual);
}

int main(void) {
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
    

    UnityBegin("test.c");
    RUN_TEST(test_1_equal_1);
    int unity_code = UnityEnd();
    jumper_sudo_exit_with_exit_code(unity_code);
}