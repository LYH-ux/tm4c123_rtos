/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-08     AHTYDHD      the first version
 */
 
#include <rtthread.h>
#include <rtdevice.h>

#define SAMPLE_UART_NAME                 "uart1"
#define ONE_DATA_MAXLEN                  64


/* the semaphore for receive information */
static struct rt_semaphore rx_sem;
static rt_device_t serial;

struct uart_protocol
{
	 unsigned char  start_flag0;
	 unsigned char  start_flag1;
	 unsigned char  function_flag;
	 unsigned char  data_length;
   unsigned char  sum;	
};

struct uart_protocol  user_protocol={0xAA,0xAA,0,0,0};

/* the ind function for rx data */
static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    /* when uart receive data , call this function ,release sem */
    if (size > 0)
    {
        rt_sem_release(&rx_sem);
    }
    return RT_EOK;
}

static char uart_sample_get_char(void)
{
    char ch;

    while (rt_device_read(serial, 0, &ch, 1) == 0)  /* when read char failed , then reset rx_sem and waiting rx_sem */
    {
        rt_sem_control(&rx_sem, RT_IPC_CMD_RESET, RT_NULL);
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}

/* function of  control command processing */
static int control_command(const char *data, const char len)
{
    if(len != 1)
    {
        return -1;
    }
		rt_uint8_t tx_buff[7]={0};
    switch (data[0])
    {
    case 0x01:
				tx_buff[0] =0xAA;
		    tx_buff[1] =0xAD;
			  tx_buff[2] =0x01;
			  tx_buff[3] =0x02;
		    tx_buff[4] =0x25;
				tx_buff[5] =0x10;
		    tx_buff[6] =0x8F;
				rt_device_write(serial, 0, tx_buff, 7);
        //rt_kprintf("control command 0x01");
        break;
    case 0x02:
        rt_kprintf("control command 0x02");
        break;
    default:
        break;
    }
    return 0;
}

/* function of read data */
static int read_data(const char*data, const char len)
{
    if(len != 1)
    {
        return -1;
    }
    switch (data[0])
    {
    case 0x01:
        rt_kprintf("read PID");
        break;
    case 0x02:
        rt_kprintf("read user defined data");
        break;
    default:
        break;
    }
    return 0;    
}


/* distribute function of uart information */
static void information_distribute(const unsigned char func, const char *data, const char len)
{
    if(data == NULL)
    {
        return ;
    }
    if( rt_strlen(data) != len)
    {
        return ;
    }
    switch(func)
    {
    case 0x01:
        control_command(data,len);
        break;
    case 0x02:
        read_data(data,len);
        break;
    default:         
        break;
    }
}

/* entry of data_parsing thread */
static void data_parsing(void)
{
    char data[ONE_DATA_MAXLEN];
    unsigned char endflag=0;
    int  i = 0;
    while (1)
    {
			    int sum = 0;
			    for(i=0; i<4 ;i++)
			    {
				    data[i] = uart_sample_get_char();
			    }
					if(data[0] == user_protocol.start_flag0 && data[1] == user_protocol.start_flag1)
					{
                        user_protocol.function_flag = data[2];
                        if( data[3] ) /*char is unsigned integer, always >= 0*/
                        {
                                user_protocol.data_length = data[3];
                                sum += user_protocol.start_flag0 + user_protocol.start_flag1 + 
                                    user_protocol.function_flag + user_protocol.data_length;
                                for(i=0; i< user_protocol.data_length; i++)
                            {
                                data[i]= uart_sample_get_char();
                                sum += data[i];
                            }
                            data[i]='\0';
                            endflag = uart_sample_get_char();
                            if(endflag == (unsigned char)(sum&0xFF))
                            {
                                information_distribute(user_protocol.function_flag,data,user_protocol.data_length);
                            }
                        }								
					}
    }
}

static int uart_data_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    char str[] = "hello RT-Thread!\r\n";

    if (argc == 2)
    {
        rt_strncpy(uart_name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);
    }

    /* find uart device */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    /* init sem */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* open uart in RX and poll */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* set rx ind function */
    rt_device_set_rx_indicate(serial, uart_rx_ind);
    /* send data */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

    /* create  serial thread */
    rt_thread_t thread = rt_thread_create("serial", (void (*)(void *parameter))data_parsing, RT_NULL, 1024, 25, 10);
    /* if create serial thread success ,then start the thread */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}

/* export to  msh CMD list */
//MSH_CMD_EXPORT(uart_data_sample, uart device sample);
//INIT_APP_EXPORT(uart_data_sample);