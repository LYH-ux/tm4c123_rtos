#ifndef   _BOARD_H_
#define   _BOARD_H_


#ifdef RT_USING_PIN
#include "drv_gpio.h"
#endif /* RT_USING_PIN */


#ifdef RT_USING_SERIAL
#include "drv_uart.h"
#endif /* RT_USING_SERIAL */

#ifdef RT_USING_PWM
#include "drv_pwm.h"
#endif /* RT_USING_PWM*/


#endif /*_BOARD_H_*/

