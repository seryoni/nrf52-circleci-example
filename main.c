#include "main.h"

/***************/
/* SYSTEM TEST */
/***************/
int main(void)
{

    uint32_t err_code = NRF_SUCCESS;

    //Configure all LEDs on board.
    bsp_board_leds_init();

    //BSP configuration for button support: button pushing will feed the dog.
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = bsp_init(BSP_INIT_BUTTONS, bsp_event_callback);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_buttons_enable();
    APP_ERROR_CHECK(err_code);
    while (1)
    {
        __SEV();
        __WFE();
        __WFE();
    }
}

/** @} */
