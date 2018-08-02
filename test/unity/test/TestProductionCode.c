#include "main.h"
#include "unity.h"
#include "jumper.h"

#define BUTTON1 (13)

void setUp(void)
{
}

void tearDown(void)
{
}

void test_CallTheButtonCallback(void) 
{
    TEST_ASSERT_FALSE(button_callback_callad);
    button_cb(); //todo: remove
    jumper_sudo_set_pin_level(BUTTON1, 1);
    TEST_ASSERT_TRUE(button_callback_callad);
}