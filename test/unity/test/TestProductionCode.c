#include "/Users/elhanan/jumper/nrf52-circleci-example/src/main.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_CallTheButtonCallback(void) 
{
    button_cb();
    TEST_ASSERT_TRUE(button_callback_callad);
}