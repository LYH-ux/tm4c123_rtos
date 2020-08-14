/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-27     AHTYDHD      the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <math.h>
#include "micro_lab.h"

int main(void)
{
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(2, PIN_MODE_OUTPUT);
		
		int i=0;
	  float point;
    while (count++)
    {
				
			 //point = 125*sin(i++*3.1415926/180)+125;
			 //micro_lib_update_OSC((char *)&point,sizeof(float));
			 //rt_thread_mdelay(100);
       rt_pin_write(2, PIN_HIGH);
       rt_thread_mdelay(50);
       rt_pin_write(2, PIN_LOW);
       rt_thread_mdelay(50); 
    }
    
    return RT_EOK;
}
