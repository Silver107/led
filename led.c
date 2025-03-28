

#include <stdio.h>
#include <string.h>
#include <rtdevice.h>
#include "drv_led.h"
#include "led.h"

#if defined(USING_LED) && defined(LED_USE_PWM)
#define DBG_TAG               "led_comm"
#define DBG_LVL               DBG_INFO
#include <rtdbg.h>

#define LED1_ID (0)
#define LED2_ID (1)

led_err_t led_open(uint8_t id)
{
    rt_err_t ret = RT_EOK;
    struct rt_device_pin_mode m;
    struct rt_device_pin_status st;
    rt_device_t device = NULL;
    rt_uint16_t led_pin = 0;
    
    if(id == LED1_ID)
    {
#if defined(USING_LED1)
        device = rt_device_find("pin");
        led_pin = LED1_POWER_IO;
#endif
    }
    else if(id == LED2_ID)
    {
#if defined(USING_LED2)
        device = rt_device_find("pin");
        led_pin = LED2_POWER_IO;
#endif
    }
    
    if (!device)
    {
        LOG_E("GPIO pin device not found at led ctrl\n");
        return RT_EIO;
    }

    ret = rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK) return ret;
    m.pin = led_pin;
    m.mode = PIN_MODE_OUTPUT;
    rt_device_control(device, 0, &m);

    st.pin = led_pin;
    st.status = 1;
    rt_device_write(device, 0, &st, sizeof(struct rt_device_pin_status));

    ret = rt_device_close(device);

    if (ret != LED_EOK)
        ret = LED_ERROR;

    return ret;
}

led_err_t led_close(uint8_t id)
{
    rt_err_t ret = RT_EOK;
#if defined (BSP_USING_PWM) && defined (LED_USE_PWM)
    struct rt_device_pin_mode m;
    struct rt_device_pin_status st;
    rt_device_t device = NULL;
    rt_uint16_t led_pin = 0;
    
    if(id == LED1_ID)
    {
#if defined(USING_LED1)
        device = rt_device_find("pin");
        led_pin = LED1_POWER_IO;
#endif
    }
    else if(id == LED2_ID)
    {
#if defined(USING_LED2)
        device = rt_device_find("pin");
        led_pin = LED2_POWER_IO;
#endif
    }

    if (!device)
    {
        LOG_E("GPIO pin device not found at led ctrl\n");
        return RT_EIO;
    }

    ret = rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK) return ret;
    m.pin = led_pin;
    m.mode = PIN_MODE_OUTPUT;
    rt_device_control(device, 0, &m);

    st.pin = led_pin;
    st.status = 0;
    rt_device_write(device, 0, &st, sizeof(struct rt_device_pin_status));

    ret = rt_device_close(device);

    if (ret != LED_EOK)
        ret = LED_ERROR;
#endif
    return ret;
}

led_err_t led_ctrl(uint8_t id, uint8_t status)
{
    rt_err_t ret = RT_EOK;
    
    if(status == 1)
    {
        return led_close(id);
    }
    else
    {
        return led_open(id);
    }
    
    return ret;
}

led_err_t led_pwm_open(uint8_t id, void *args)
{
    rt_err_t ret = RT_EOK;
#if defined (BSP_USING_PWM) && defined (LED_USE_PWM)
    struct rt_device_pwm *led_device = NULL;
    int channel = 0;
    rt_uint32_t period = 0 ;
    
    if(id == LED1_ID)
    {
#if defined(USING_LED1)
        led_device = (struct rt_device_pwm *)rt_device_find(LED1_INTERFACE_NAME);
        channel = LED1_CHANEL_NUM;
        period = LED1_PERIOD * 1000 * 1000;
        rt_kprintf("+++++++++++++000000++++++++++\n");
        HAL_PIN_Set(PAD_PA31, GPTIM1_CH2, PIN_NOPULL, 1); //PWM2
        rt_kprintf("+++++++++++++444444++++++++++\n");
#endif
    }
    else if(id == LED2_ID)
    {
#if defined(USING_LED2)
        led_device = (struct rt_device_pwm *)rt_device_find(LED2_INTERFACE_NAME);
        channel = LED2_CHANEL_NUM;
        period = LED2_PERIOD * 1000 * 1000;
        rt_kprintf("+++++++++++++11111++++++++++\n");
        HAL_PIN_Set(PAD_PB13, GPTIM3_CH2, PIN_NOPULL, 0); //PWM4
        //HAL_PIN_Set(PAD_PB13, GPIO_B13, PIN_PULLDOWN, 0);
        rt_kprintf("+++++++++++++22222++++++++++\n");
#endif
    }
    
    if (!led_device)
    {
        rt_kprintf("led_device[%d]not found \n",id);
    }
    else
    {
        rt_kprintf("led_pwm_open cyc %d %d %d\n",*(uint32_t *)args, period, (period / 100) * (*(uint32_t *)args));
        rt_pwm_set(led_device, channel, period, (period / 100) * (*(uint32_t *)args));
        ret = rt_pwm_enable(led_device, channel);
    }
#endif
    return ret;
}

led_err_t led_pwm_close(uint8_t id, void *args)
{
    rt_err_t ret = RT_EOK;
#if defined (BSP_USING_PWM) && defined (LED_USE_PWM)
    struct rt_device_pwm *led_device = NULL;
    int channel = 0;
    
    if(id == LED1_ID)
    {
#if defined(USING_LED1)
        led_device = (struct rt_device_pwm *)rt_device_find(LED1_INTERFACE_NAME);
        channel = LED1_CHANEL_NUM;
#endif
    }
    else if(id == LED2_ID)
    {
#if defined(USING_LED2)
        led_device = (struct rt_device_pwm *)rt_device_find(LED2_INTERFACE_NAME);
        channel = LED2_CHANEL_NUM;
#endif
    }
    
    if (!led_device)
    {
        rt_kprintf("led_device[%d]not found \n",id);
    }
    else
    {
        ret = rt_pwm_disable(led_device, channel);
    }
#endif
    return ret;
}

led_err_t led_control(struct rt_led_device *led_device, int cmd, void *args)
{
    led_err_t ret = LED_EOK;

    switch (cmd)
    {
    case LED_CONTROL_CLOSE_DEVICE:
    {
        //ret = led_close(led_device->id);
    }
    break;

    case LED_CONTROL_OPEN_DEVICE:
    {
        //ret = led_open(led_device->id);
    }
    break;

    case LED_CONTROL_CTRL_IO:
    {
        ret = led_ctrl(led_device->id, *(uint8_t *)args);

    }
    break;

    case LED_CONTROL_PWM_OPEN:
    {
        rt_kprintf("++++++++++LED_CONTROL_PWM_OPEN\n");
        ret = led_pwm_open(led_device->id, args);
    }
    break;

    case LED_CONTROL_PWM_CLOSE:
    {
        ret = led_pwm_close(led_device->id, args);
    }
    break;

    default:
        break;
    }
    return ret;
}

void led_pin_init()
{

    return;
}

static const struct rt_led_ops led_comm_ops =
{
    .control = led_control
};

int led_init(void)
{
    rt_kprintf("++++++++++led_init\n");
    
    rt_err_t ret = RT_EOK;

#if defined(USING_LED1)
    rt_hw_led_init(&led_comm_ops, LED1_ID);
#endif

#if defined(USING_LED2)
    rt_hw_led_init(&led_comm_ops, LED2_ID);
#endif

    return LED_EOK;
}

INIT_COMPONENT_EXPORT(led_init);
#endif

