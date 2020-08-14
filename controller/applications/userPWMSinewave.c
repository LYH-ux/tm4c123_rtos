/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-13     AHTYDHD      the first version
*/
 
#include <rtthread.h>
#include <rtdevice.h>

#define PWM_DEV_NAME  "pwm3"
#define SAMPLE_CHANNEL  1
#define SIGNAL_CHANNEL  2

struct rt_device_pwm *pwm_dev;    /* pwm device handler*/

static int pwm_sinewave(void)
{
    rt_uint32_t  period,signal_pulse,sample_pulse;

    period = 400000;
    signal_pulse = 200000;
    sample_pulse = 20000;

    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return RT_ERROR;
    }
		/* enable pwm output channel*/
    rt_pwm_enable(pwm_dev, SIGNAL_CHANNEL);
    rt_pwm_enable(pwm_dev, SAMPLE_CHANNEL);
    /* set period and pulse width */
    rt_pwm_set(pwm_dev, SIGNAL_CHANNEL, period, signal_pulse);
    rt_pwm_set(pwm_dev, SAMPLE_CHANNEL, period, sample_pulse);
    return RT_EOK;
}
INIT_APP_EXPORT(pwm_sinewave);