#include "bme280.c"
#include "jumper.h"
#include "main.h"
#include "unity.h"
#include "button.h"


#define BUTTON1 (13)

void setUp(void)
{
    button_callback_callad = false;
}

void tearDown(void)
{
}

void test_CallTheButtonCallback(void) 
{
    TEST_ASSERT_FALSE(button_callback_callad);
    bsp_event_callback(BSP_EVENT_KEY_0); 
    TEST_ASSERT_TRUE(button_callback_callad);
}

void test_SetButtonPinLevel(void) 
{
    // TEST_ASSERT_FALSE(button_callback_callad);
    // jumper_sudo_set_pin_level(BUTTON1, 0);
    // TEST_ASSERT_TRUE(button_callback_callad); //todo: uncomment
}

void test_parse_sensor_data(void) 
{
    uint8_t reg_data[] = {80, 177, 16, 133 ,249, 224, 111, 13};
    struct bme280_uncomp_data uncomp_data = {0};
    parse_sensor_data(reg_data, &uncomp_data);

    TEST_ASSERT_EQUAL(uncomp_data.pressure, 330513);
    TEST_ASSERT_EQUAL(uncomp_data.temperature, 548766);
    TEST_ASSERT_EQUAL(uncomp_data.humidity, 28429);
}