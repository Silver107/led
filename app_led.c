/**
  ******************************************************************************
  * @file   app_comm.c
  * @author Sifli software development team
  * @brief sensor service source.
 *
  ******************************************************************************
*/
/**
 * @attention
 * Copyright (c) 2019 - 2022,  Sifli Technology
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Sifli integrated circuit
 *    in a product or a software update for such product, must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Sifli nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Sifli integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY SIFLI TECHNOLOGY "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL SIFLI TECHNOLOGY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "app_led.h"
#include "app_thread.h"
#include "rtdevice.h"
#include "app_db.h"
#include "app_comm_custom.h"

#if defined(USING_LED)
    #include "drv_led.h"
#endif

#if defined(USING_LED)
static rt_bool_t led_request[APP_LED_NUM_MAX] = {RT_FALSE};
static rt_device_t led_device[APP_LED_NUM_MAX] = {NULL};
static const char *led_device_name[APP_LED_NUM_MAX] = 
{
    LED1_DEVICE_NAME,
    LED2_DEVICE_NAME
};

static uint8_t app_led_pm_request(uint8_t led_id)
{
#ifdef RT_USING_PM
    if(led_id >= APP_LED_NUM_MAX) return RT_ERROR;
    
    rt_bool_t led_request_tmp = RT_FALSE;
    for(uint8_t i = 0; i < APP_LED_NUM_MAX; i++)
    {
        if(led_request[i])
            led_request_tmp = RT_TRUE;
    }
    if (!led_request_tmp)
    {
        rt_kprintf("+++++++++rt_pm_hw_device_start\n");
        rt_pm_request(PM_SLEEP_MODE_IDLE);
        rt_pm_hw_device_start();
    }
    led_request[led_id] = RT_TRUE;
#endif
    return RT_EOK;
}

static uint8_t app_led_pm_release(uint8_t led_id)
{
#ifdef RT_USING_PM
    if(led_id >= APP_LED_NUM_MAX) return RT_ERROR;
    
    rt_bool_t led_request_tmp = RT_FALSE;
    for(uint8_t i = 0; i < APP_LED_NUM_MAX; i++)
    {
        if(i == led_id) continue;
        if(led_request[i])
            led_request_tmp = RT_TRUE;
    }
    if (!led_request_tmp)
    {
        rt_kprintf("+++++++++rt_pm_hw_device_stop\n");
        rt_pm_hw_device_stop();
        rt_pm_release(PM_SLEEP_MODE_IDLE);
    }
    led_request[led_id] = RT_FALSE;
#endif
    return RT_EOK;
}

uint8_t app_led_iocontrl(uint8_t led_id, uint8_t status)
{
    rt_err_t ret = RT_EOK;
    if(led_id >= APP_LED_NUM_MAX) return RT_ERROR;

    if (led_device[led_id] == NULL) return RT_ERROR;

    ret = rt_device_control(led_device[led_id], LED_CONTROL_CTRL_IO, (void *)&status);

    return ret;
}

uint8_t app_led_power_onoff(uint8_t led_id, uint8_t status)
{
    rt_err_t ret = RT_EOK;
    if(led_id >= APP_LED_NUM_MAX) return RT_ERROR;

    rt_kprintf("app_led_power: %s \n", status ? "on" : "off");

    led_device[led_id] = rt_device_find(led_device_name[led_id]);
    if (led_device[led_id] == NULL)
    {
        rt_kprintf("find led %d fail\n", led_id);
        return RT_ERROR;
    }

    if (status)
    {
        ret = rt_device_control(led_device[led_id], LED_CONTROL_OPEN_DEVICE, RT_NULL);
    }
    else
    {
        ret = rt_device_control(led_device[led_id], LED_CONTROL_CLOSE_DEVICE, RT_NULL);
    }

    return ret;
}

void app_turn_on_led(uint8_t led_id)
{
    if(led_id >= APP_LED_NUM_MAX) return;
    
    app_led_pm_request(led_id);

#if defined (BSP_USING_PWM) && defined (LED_USE_PWM)
    if (led_device[led_id] == NULL) return;

    rt_kprintf("+++++++on+++++++++ id %d\n", led_id);
    uint32_t duty_cyc = 50;
    rt_device_control(led_device[led_id], LED_CONTROL_PWM_OPEN, (void *)&duty_cyc);
#endif

}

void app_turn_off_led(uint8_t led_id)
{
    if(led_id >= APP_LED_NUM_MAX) return;
    
#if defined (BSP_USING_PWM) && defined (LED_USE_PWM)
    if (led_device[led_id] == NULL) return;

    rt_kprintf("++++++off++++++++++ id %d\n", led_id);

    rt_device_control(led_device[led_id], LED_CONTROL_PWM_CLOSE, RT_NULL);
#endif

    app_led_pm_release(led_id);
}

#endif

void app_led_on(uint8_t led_id)
{
#if defined(USING_LED) && !defined(BSP_USING_PC_SIMULATOR)
    app_led_control_t led_control;
    led_control.id = led_id;
    led_control.status = 1;
    hl_if_data_t *msg = service_fill_msg(APP_INT, LOCAL_SETTING_LED_CONTROL, (uint8_t *)&led_control, sizeof(led_control));
    rt_err_t err = send_msg_to_preprocess_thread(msg, sizeof(msg));
#endif
}

void app_led_off(uint8_t led_id)
{
#if defined(USING_LED) && !defined(BSP_USING_PC_SIMULATOR)
    rt_kprintf("%s|%d\r\n",__FUNCTION__,__LINE__);
    app_turn_off_led(led_id);
    app_led_power_onoff(led_id, 0);
#endif
}

