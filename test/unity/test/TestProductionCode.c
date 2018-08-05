#include "main.h"
#include "unity.h"
#include "jumper.h"

#define BUTTON1 (13)

void setUp(void)
{
    button_callback_callad = 0;
}

void tearDown(void)
{
}

void test_CallTheButtonCallback(void) 
{
    TEST_ASSERT_FALSE(button_callback_callad);
    button_cb(); 
    TEST_ASSERT_TRUE(button_callback_callad);
}

void test_SetButtonPinLevel(void) 
{
    TEST_ASSERT_FALSE(button_callback_callad);
    jumper_sudo_set_pin_level(BUTTON1, 0);
    // TEST_ASSERT_TRUE(button_callback_callad); //todo: uncomment
}