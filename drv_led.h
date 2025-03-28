/*********************
 *      INCLUDES
 *********************/
#ifndef _DRV_LED_H
#define _DRV_LED_H

#include <rtthread.h>
#include "rtdevice.h"
#include <rthw.h>
#include <drv_common.h>

#define  LED_DEVICE_MAX_CNT  (2)
#define  LED1_DEVICE_NAME    "led1"
#define  LED2_DEVICE_NAME    "led2"

int rt_hw_led_init(const struct rt_led_ops *ops, uint8_t id);
#endif /* _DRV_LED_H */


