/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-14     Rice       the first version
 * 2020-07-21     AHTYDHD    modify file to adapt tm4c123bsp
 */
#include <rtdevice.h>
#include "micro_lab.h"

#define DBG_TAG "micro_lab"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define MICRO_LIB_DEV_NAME "uart1"

rt_device_t micro_lib_dev = RT_NULL;


static struct micro_lib_tx_packet
{
    rt_uint16_t head;
    rt_uint8_t  version;
    rt_uint16_t organization;
    rt_uint16_t section;
    rt_uint16_t data_point;
    rt_uint16_t component_type;
    rt_uint16_t component_num;
    rt_uint16_t msg_len;
    rt_uint8_t  msg[4];
    rt_uint8_t  crc;
}__attribute__((packed)) micro_lib_tx_packet;

#define MICRO_LIB_TX_PACKET_SIZE    sizeof(struct micro_lib_tx_packet)

#define MICRO_LIB_TX_PROTOCOL_SIZE    (member_offset(struct micro_lib_tx_packet, msg) + 1)

static struct micro_lib_rx_packet
{
    rt_uint16_t head;
    rt_uint8_t  version;
    rt_uint16_t organization;
    rt_uint16_t section;
    rt_uint16_t data_point;
    rt_uint16_t component_type;
    rt_uint16_t component_num;
    rt_uint8_t  msg_type;
    rt_uint16_t msg_len;
    rt_uint8_t  *msg;
    rt_uint8_t  crc;
}__attribute__((packed)) micro_lib_rx_packet;
#define MICRO_LIB_RX_PACKET_SIZE    sizeof(struct micro_lib_rx_packet)

void pack_tx_packet(rt_uint16_t organization,
                           rt_uint16_t section,
                           rt_uint16_t data_point,
                           COMPONENT_TYPE component_type,
                           rt_uint16_t component_num,
                           rt_uint16_t msg_type,
                           rt_uint8_t *msg,
                           rt_uint16_t msg_len,
                           rt_uint8_t *tx_buff,
                           rt_uint16_t *tx_len)
{
    struct micro_lib_tx_packet *tx_packet = (struct micro_lib_tx_packet *)tx_buff;
    rt_uint8_t crc_num = 0x00;

    if((MICRO_LIB_TX_PROTOCOL_SIZE + msg_len + 1) > SEND_MSG_SIZE)
    {
        return;
    }

    tx_packet->head             = MICRO_LIB_HEAD;
    tx_packet->version          = MICRO_LIB_VERSION;
    tx_packet->organization     = organization;
    tx_packet->section          = section;
    tx_packet->data_point       = data_point;
    tx_packet->component_type   = component_type;
    tx_packet->component_num    = component_num;
    tx_packet->msg_len          = msg_len;
    rt_memcpy(tx_packet->msg, msg, msg_len);

    for(int i = 0; i < (MICRO_LIB_TX_PROTOCOL_SIZE + msg_len - 1); ++i)
    {
        crc_num += (rt_uint8_t)tx_buff[i];
    }
    tx_packet->crc = crc_num;

    *tx_len = MICRO_LIB_TX_PROTOCOL_SIZE + msg_len;
}

void micro_lib_update_canvas(COMPONENT_TYPE component_type,
                             rt_uint16_t component_num,
                             rt_uint8_t *msg,
                             rt_uint16_t msg_len)
{
    rt_uint8_t tx_buff[SEND_MSG_SIZE];
    rt_uint16_t tx_buff_len;

    pack_tx_packet(MICRO_LIB_ORGANIZATION,
                   MICRO_LIB_SECTION,
                   MICRO_LIB_DATAPOINT,
                   component_type,
                   component_num,
                   NONE_MSG,
                   msg,
                   msg_len,
                   tx_buff,
                   &tx_buff_len);

    rt_device_write(micro_lib_dev, 0, tx_buff, tx_buff_len);
}

void micro_lib_update_OSC(char *data, unsigned short len)
{
    if(len < 4 || (len % 4) != 0)
    {
        return;
    }
    rt_device_write(micro_lib_dev, 0, data, len);
}

int micro_lib_init()
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    micro_lib_dev = rt_device_find(MICRO_LIB_DEV_NAME);
    if(micro_lib_dev == RT_NULL)
    {
        LOG_E("find micro lib fail");
        return RT_ERROR;
    }

    config.baud_rate = BAUD_RATE_115200;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.bufsz     = 128;
    config.parity    = PARITY_NONE;

    rt_device_control(micro_lib_dev, RT_DEVICE_CTRL_CONFIG, &config);

    rt_device_open(micro_lib_dev, RT_DEVICE_FLAG_INT_RX);

    return RT_EOK;
}
//INIT_APP_EXPORT(micro_lib_init);