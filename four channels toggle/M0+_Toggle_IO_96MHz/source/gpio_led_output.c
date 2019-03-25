/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_BOARD_TEST_LED_PORT 1U
#define APP_BOARD_TEST_LED_PIN 10U
#define APP_SW_PORT BOARD_SW1_GPIO_PORT
#define APP_SW_PIN BOARD_SW1_GPIO_PIN


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 100000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t port_state = 0;
    uint32_t KEY_value = 0;
		register uint32_t *p =(uint32_t *)0x4008E300;
		register uint32_t myr0 = 0x002400a0;
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };
    /* Define the init structure for the key pin*/
    gpio_pin_config_t key_row_config = {
        kGPIO_DigitalOutput, 0,
    };
    gpio_pin_config_t key_column_config = {
        kGPIO_DigitalInput, 0,
    };
		
		
    /* Board pin, clock, debug console init */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
    /* enable clock for GPIO*/
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);

    BOARD_InitPins();
    BOARD_BootClockFROHF96M();
    BOARD_InitDebugConsole();

    /* Print a note to terminal. */
    PRINTF("\r\n GPIO Driver example\r\n");
    PRINTF("\r\n The LED is taking turns to shine.\r\n");

//    /* Init SW GPIO PORT. */
//    GPIO_PortInit(GPIO, APP_SW_PORT);

//    /* Init output LED GPIO. */
//    GPIO_PortInit(GPIO, APP_BOARD_TEST_LED_PORT);
//    GPIO_PinInit(GPIO, APP_BOARD_TEST_LED_PORT, APP_BOARD_TEST_LED_PIN, &led_config);
//    GPIO_PinWrite(GPIO, APP_BOARD_TEST_LED_PORT, APP_BOARD_TEST_LED_PIN, 1);

    /* Port masking */
//    GPIO_PortMaskedSet(GPIO, APP_BOARD_TEST_LED_PORT, 0x0000FFFF);
//    GPIO_PortMaskedWrite(GPIO, APP_BOARD_TEST_LED_PORT, 0xFFFFFFFF);
//    port_state = GPIO_PortRead(GPIO, APP_SW_PORT);
//    PRINTF("\r\n Standard port read: %x\r\n", port_state);
//    port_state = GPIO_PortMaskedRead(GPIO, APP_SW_PORT);
//    PRINTF("\r\n Masked port read: %x\r\n", port_state);

    /* Init key PORT. */
    GPIO_PortInit(GPIO, BOARD_KEY_PORT);
    /* Init output key row GPIO. */
    GPIO_PortInit(GPIO, BOARD_KEY_PORT);
    GPIO_PinInit(GPIO, BOARD_KEY_PORT, KEY_ROW_1, &key_row_config);
    GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_1, 0);
    /* Init output key rozw GPIO. */
    GPIO_PinInit(GPIO, BOARD_KEY_PORT, KEY_ROW_2, &key_row_config);
    GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_2, 0);
    /* Init output key row GPIO. */
    GPIO_PinInit(GPIO, BOARD_KEY_PORT, KEY_ROW_3, &key_row_config);
    GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_3, 0);
    /* Init output key row GPIO. */
    GPIO_PinInit(GPIO, BOARD_KEY_PORT, KEY_ROW_4, &key_row_config);
    GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_4, 0);
		
    /* Init input key column GPIO. */
    GPIO_PinInit(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1, &key_column_config);
    GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1, 0);
    /* Init input key column GPIO. */
    GPIO_PinInit(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2, &key_column_config);
    GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2, 0);
    /* Init input key column GPIO. */
    GPIO_PinInit(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3, &key_column_config);
    GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3, 0);
    /* Init input key column GPIO. */
    GPIO_PinInit(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4, &key_column_config);
    GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4, 0);

		
    while (1){*p = myr0;*p = myr0;*p = myr0;*p = myr0;*p = myr0;*p = myr0;}
		
			
//				GPIO->NOT[0]=0x002400A0;
//				KEY_value = 0;
//				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_1, 1);			
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1)& (1<<KEY_COLUMN_1));
//				KEY_value= KEY_value << 0;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2)& (1<<KEY_COLUMN_2));
//				KEY_value= KEY_value << 1;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3)& (1<<KEY_COLUMN_3));
//				KEY_value= KEY_value << 2;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4)& (1<<KEY_COLUMN_4));
//				KEY_value= KEY_value << 3;
//				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_1, 0);			
//			
//				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_2, 1);			
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1)& (1<<KEY_COLUMN_1));
//				KEY_value= KEY_value << 4;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2)& (1<<KEY_COLUMN_2));
//				KEY_value= KEY_value << 5;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3)& (1<<KEY_COLUMN_3));
//				KEY_value= KEY_value << 6;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4)& (1<<KEY_COLUMN_4));
//				KEY_value= KEY_value << 7;
//				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_2, 0);			

//				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_3, 1);			
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1)& (1<<KEY_COLUMN_1));
//				KEY_value= KEY_value << 8;\
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2)& (1<<KEY_COLUMN_2));
//				KEY_value= KEY_value << 9;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3)& (1<<KEY_COLUMN_3));
//				KEY_value= KEY_value <<10;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4)& (1<<KEY_COLUMN_4));
//				KEY_value= KEY_value << 11;
//				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_3, 0);			

//				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_4, 1);			
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1)& (1<<KEY_COLUMN_1));
//				KEY_value= KEY_value << 12;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2)& (1<<KEY_COLUMN_2));
//				KEY_value= KEY_value << 13;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3)& (1<<KEY_COLUMN_3));
//				KEY_value= KEY_value << 14;
//				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4)& (1<<KEY_COLUMN_4));
//				KEY_value= KEY_value << 15;
//				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_4, 0);			


			
			
//        port_state = GPIO_PortRead(GPIO, APP_SW_PORT);
//        if (!(port_state & (1 << APP_SW_PIN)))
//        {
//            PRINTF("\r\n Port state: %x\r\n", port_state);
//            GPIO_PortToggle(GPIO, APP_BOARD_TEST_LED_PORT, 1u << APP_BOARD_TEST_LED_PIN);
//        }
//        delay();
}
