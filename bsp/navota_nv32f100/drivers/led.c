/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2014, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-11-23     Bright      first implementation
 */

#include "led.h"
/* RT_USING_COMPONENTS_INIT */
#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif

/*
    LED: P3.6
*/
#define LED_PORT P3
#define LED_DATA(dat) (P36 = dat)
#define LED_BIT BIT6

/* Initial led gpio pin  */
int rt_hw_led_init(void)
{
    /* Configure the GPIO_LED pin */
    GPIO_Init(GPIOB, GPIO_PTE7_MASK, GPIO_PinOutput);
    GPIO_Init(GPIOB, GPIO_PTH1_MASK, GPIO_PinOutput);
    GPIO_Init(GPIOB, GPIO_PTH2_MASK, GPIO_PinOutput);
    
    GPIO_PinSet(GPIO_PTE7);
    GPIO_PinSet(GPIO_PTH1);
    GPIO_PinSet(GPIO_PTH2);
    
    return 0;
}

void rt_hw_led_on(void)
{
    GPIO_PinClear(GPIO_PTE7);
}

void rt_hw_led_off(void)
{
    GPIO_PinSet(GPIO_PTE7);
}

/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_led_init);