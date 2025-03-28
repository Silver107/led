#ifndef __LED_DEVICE_H__
#define __LED_DEVICE_H__
#include <rtthread.h>

#define LED_DEVICE_FLAG_OPEN (0x80)

typedef enum
{
    LED_CONTROL_CLOSE_DEVICE = 0x20 + 1,     /**< close led device */
    LED_CONTROL_OPEN_DEVICE,                 /**< open led device */
    LED_CONTROL_CTRL_IO,
    LED_CONTROL_PWM_OPEN,
    LED_CONTROL_PWM_CLOSE,
    LED_CONTROL_LUMINANCE,

    LED_CONTROL_CMD_MAX
} led_control_cmd_t;

typedef enum
{
    LED_STATE_POWER_OFF = 0,
    LED_STATE_POWER_ON,
} led_state_t;

typedef enum
{
    LED_EOK = 0,
    LED_ERROR,
} led_err_t;

typedef struct
{
    led_state_t last_state;
    led_state_t cur_state;
} led_status_t;

typedef struct rt_led_device
{
    struct rt_device   parent;
    //rt_mutex_t handle_lock;
    led_status_t status;
    uint8_t ctrl_status;
    const struct rt_led_ops *ops;
    uint8_t id;
} rt_led_t;

typedef led_err_t (*led_control_cb)(struct rt_led_device *dev_handle, int cmd, void *arg);

struct rt_led_ops
{
    led_control_cb control;
};

rt_err_t rt_led_register(struct rt_led_device *dev_handle, const char *name);
#endif
