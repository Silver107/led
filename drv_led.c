

#include <stdio.h>
#include <string.h>
#include "drv_led.h"
#define DBG_TAG    "drv_led"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>


#ifdef USING_LED
struct rt_led_device led_dev[LED_DEVICE_MAX_CNT] = {0};
const char *led_dev_name[LED_DEVICE_MAX_CNT] = 
{
    LED1_DEVICE_NAME,
    LED2_DEVICE_NAME,
};

int rt_hw_led_init(const struct rt_led_ops *ops, uint8_t id)
{
    rt_err_t result = 0;

    if (id >= LED_DEVICE_MAX_CNT)
    {
        rt_kprintf("LED device %s not found!\n");
        return RT_ERROR;
    }
    
    led_dev[id].ops = ops;
    led_dev[id].id = id;

    led_dev[id].status.cur_state = LED_STATE_POWER_OFF;
    led_dev[id].status.last_state = LED_STATE_POWER_OFF;
    result = rt_led_register(&led_dev[id], led_dev_name[id]);
    if (result != RT_EOK)
    {
        rt_kprintf("LED %s register failed! error=%d\n", led_dev_name[id], result);
        return result;
    }
    
    return result;
}

#endif /* USING_LED */

