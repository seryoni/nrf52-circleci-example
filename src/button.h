#include "bsp.h"

bool button_callback_callad = false;

void bsp_event_callback(bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_KEY_0:
            button_callback_callad = true;
            break;
        default :
            //Do nothing.
            break;
    }
}