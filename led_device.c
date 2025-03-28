/*
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>


#define DBG_TAG    "led_device"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>

static rt_err_t rt_led_control(struct rt_device *dev, int cmd, void *args);


static led_err_t rt_led_close(struct rt_device *dev)
{
    struct rt_led_device *mt_handle = RT_NULL;
    led_err_t ret = LED_EOK;
    RT_ASSERT(dev != RT_NULL);
    mt_handle = (struct rt_led_device *)dev;
    if (!(dev->open_flag & LED_DEVICE_FLAG_OPEN))
    {
        return ret;
    }

    ret = mt_handle->ops->control(mt_handle, LED_CONTROL_CLOSE_DEVICE, RT_NULL);
    if (ret == RT_EOK)
    {
        dev->open_flag &= ~LED_DEVICE_FLAG_OPEN;
        mt_handle->status.last_state = mt_handle->status.cur_state;
        mt_handle->status.cur_state = LED_STATE_POWER_OFF;
    }
//    LOG_I("rt_led_close ret:%d flag:%X", ret, dev->open_flag);
    return ret;
}

static  led_err_t rt_led_open(struct rt_device *dev)
{
    struct rt_led_device *mt_handle = RT_NULL;
    led_err_t ret = LED_EOK;
    RT_ASSERT(dev != RT_NULL);
    mt_handle = (struct rt_led_device *)dev;
    if (dev->open_flag & LED_DEVICE_FLAG_OPEN)
    {
        return ret;
    }
    ret = mt_handle->ops->control(mt_handle, LED_CONTROL_OPEN_DEVICE, RT_NULL);
    if (LED_EOK == ret)
    {
        dev->open_flag |= LED_DEVICE_FLAG_OPEN;
        mt_handle->status.last_state = mt_handle->status.cur_state;
        mt_handle->status.cur_state = LED_STATE_POWER_ON;
    }
    //dev->open_flag &= ~BT_DEVICE_FLAG_OPEN;
//    LOG_I("rt_led_open ret:%d flag:%X", ret, dev->open_flag);
    return ret;
}

static  led_err_t rt_led_ctrl(struct rt_device *dev, void *args)
{
    struct rt_led_device *mt_handle = RT_NULL;
    led_err_t ret = LED_EOK;
    RT_ASSERT(dev != RT_NULL);
    mt_handle = (struct rt_led_device *)dev;
    if (!(dev->open_flag & LED_DEVICE_FLAG_OPEN))
    {
        return ret;
    }

    ret = mt_handle->ops->control(mt_handle, LED_CONTROL_CTRL_IO, args);
//    LOG_I("rt_led_ctrl ret:%d flag:%X *args:%d", ret, dev->open_flag, *(uint8_t *)args);
    return ret;
}

static  led_err_t rt_led_pwm_open(struct rt_device *dev, void *args)
{
    struct rt_led_device *mt_handle = RT_NULL;
    led_err_t ret = LED_EOK;
    RT_ASSERT(dev != RT_NULL);
    mt_handle = (struct rt_led_device *)dev;
    if (!(dev->open_flag & LED_DEVICE_FLAG_OPEN))
    {
        return ret;
    }

    ret = mt_handle->ops->control(mt_handle, LED_CONTROL_PWM_OPEN, args);
    LOG_I("rt_led_pwm_open ret:%d flag:%X", ret, dev->open_flag);
    return ret;
}

static  led_err_t rt_led_pwm_close(struct rt_device *dev, void *args)
{
    struct rt_led_device *mt_handle = RT_NULL;
    led_err_t ret = LED_EOK;
    RT_ASSERT(dev != RT_NULL);
    mt_handle = (struct rt_led_device *)dev;
    if (!(dev->open_flag & LED_DEVICE_FLAG_OPEN))
    {
        return ret;
    }

    ret = mt_handle->ops->control(mt_handle, LED_CONTROL_PWM_CLOSE, args);
    LOG_I("rt_led_pwm_close ret:%d flag:%X", ret, dev->open_flag);
    return ret;
}

static led_err_t rt_led_pwm_level(struct rt_device *dev, void *args){

    struct rt_led_device *mt_handle = RT_NULL;
    led_err_t ret = LED_EOK;
    RT_ASSERT(dev != RT_NULL);
    mt_handle = (struct rt_led_device *)dev;
    if (!(dev->open_flag & LED_DEVICE_FLAG_OPEN))
    {
        return ret;
    }

    ret = mt_handle->ops->control(mt_handle, LED_CONTROL_LUMINANCE, args);
//    LOG_I("rt_led_pwm_close ret:%d flag:%X", ret, dev->open_flag);
    return ret;
}

static rt_err_t rt_led_control(struct rt_device *dev,
                                 int              cmd,
                                 void             *args)
{
    led_err_t ret = LED_EOK;
    struct rt_led_device *mt_handle;
    RT_ASSERT(dev != RT_NULL);
    mt_handle = (struct rt_led_device *)dev;
    switch (cmd)
    {
    case LED_CONTROL_OPEN_DEVICE:
    {
        ret = rt_led_open(dev);
    }
    break;

    case LED_CONTROL_CLOSE_DEVICE:
    {
        ret = rt_led_close(dev);
    }
    break;

    case LED_CONTROL_CTRL_IO:
    {
        ret = rt_led_ctrl(dev, args);
    }
    break;

    case LED_CONTROL_PWM_OPEN:
    {
        rt_kprintf("++++++++++LED_CONTROL_PWM_OPEN111 %d\n", mt_handle->id);
        ret = rt_led_pwm_open(dev, args);
    }
    break;

    case LED_CONTROL_PWM_CLOSE:
    {
        ret = rt_led_pwm_close(dev, args);
    }
    break;

    case LED_CONTROL_LUMINANCE:
    {
        ret = rt_led_pwm_level(dev, args);
    }
    break;

    default :
        break;
    }

    return ret;
}



#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops led_ops =
{
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    rt_led_control
};
#endif

/*
 * bt register
 */
__ROM_USED rt_err_t rt_led_register(struct rt_led_device *dev_handle, const char *name)
{
    rt_err_t ret;
    struct rt_device *device;
    RT_ASSERT(dev_handle != RT_NULL);

    device = &(dev_handle->parent);

    device->type        = RT_Device_Class_Miscellaneous;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    device->ops         = &led_ops;
#else
    device->init        = RT_NULL;
    device->open        = RT_NULL;
    device->close       = RT_NULL;
    device->read        = RT_NULL;
    device->write       = RT_NULL;
    device->control     = rt_led_control;
#endif
    device->user_data   = RT_NULL;
    /* register a Miscellaneous device */
    ret = rt_device_register(device, name, RT_DEVICE_FLAG_RDWR);
    return ret;
}
