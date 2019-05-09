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


#define PLL_SSCG0_MDEC_VAL_P (0U)                                /* MDEC is in bits  16 downto 0 */
#define PLL_SSCG0_MDEC_VAL_M (0x1FFFFUL << PLL_SSCG0_MDEC_VAL_P) /* NDEC is in bits  9 downto 0 */
#define PLL_NDEC_VAL_P (0U)                                      /* NDEC is in bits  9:0 */
#define PLL_NDEC_VAL_M (0x3FFUL << PLL_NDEC_VAL_P)
#define PLL_PDEC_VAL_P (0U) /* PDEC is in bits 6:0 */
#define PLL_PDEC_VAL_M (0x3FFUL << PLL_PDEC_VAL_P)
#define NVALMAX (0x100U)
#define PVALMAX (0x20U)
#define MVALMAX (0x8000U)


#define LIB_TRACE ;

/* Find encoded NDEC value for raw N value, max N = NVALMAX */
static uint32_t pllEncodeN(uint32_t N)
{
    uint32_t x, i;

    /* Find NDec */
    switch (N)
    {
        case 0U:
            x = 0xFFFU;
            break;

        case 1U:
            x = 0x302U;
            break;

        case 2U:
            x = 0x202U;
            break;

        default:
            x = 0x080U;
            for (i = N; i <= NVALMAX; i++)
            {
                x = (((x ^ (x >> 2U) ^ (x >> 3U) ^ (x >> 4U)) & 1U) << 7U) | ((x >> 1U) & 0x7FU);
            }
            break;
    }

    return x & (PLL_NDEC_VAL_M >> PLL_NDEC_VAL_P);
}

static uint32_t pllEncodeM(uint32_t M)
{
    uint32_t i, x;

    /* Find MDec */
    switch (M)
    {
        case 0U:
            x = 0xFFFFFU;
            break;

        case 1U:
            x = 0x18003U;
            break;

        case 2U:
            x = 0x10003U;
            break;

        default:
            x = 0x04000U;
            for (i = M; i <= MVALMAX; i++)
            {
                x = (((x ^ (x >> 1U)) & 1U) << 14U) | ((x >> 1U) & 0x3FFFU);
            }
            break;
    }

    return x & (PLL_SSCG0_MDEC_VAL_M >> PLL_SSCG0_MDEC_VAL_P);
}

/* Find encoded PDEC value for raw P value, max P = PVALMAX */
//static uint32_t pllEncodeP(uint32_t P)
//{
//    uint32_t x, i;

//    /* Find PDec */
//    switch (P)
//    {
//        case 0U:
//            x = 0xFFU;
//            break;

//        case 1U:
//            x = 0x62U;
//            break;

//        case 2U:
//            x = 0x42U;
//            break;

//        default:
//            x = 0x10U;
//            for (i = P; i <= PVALMAX; i++)
//            {
//                x = (((x ^ (x >> 2U)) & 1U) << 4U) | ((x >> 1U) & 0xFU);
//            }
//            break;
//    }

//    return x & (PLL_PDEC_VAL_M >> PLL_PDEC_VAL_P);
//}


/* Find encoded PDEC value for raw P value, max P = PVALMAX */
static uint32_t pllEncodeP(uint32_t P)
{
    uint32_t x, i;

    /* Find PDec */
    switch (P)
    {
        case 0U:
            x = 0xFFU;
            break;

        case 1U:
            x = 0x62U;
            break;

        case 2U:
            x = 0x42U;
            break;

        default:
            x = 0x10U;
            for (i = P; i <= PVALMAX; i++)
            {
                x = (((x ^ (x >> 2U)) & 1U) << 4U) | ((x >> 1U) & 0xFU);
            }
            break;
    }

    return x & (PLL_PDEC_VAL_M >> PLL_PDEC_VAL_P);
}


/* select 
    0x0 FRO 12 MHz (fro_12m)
    0x1 CLKIN (clk_in)
*/

uint32_t SetupSystemPLL(uint32_t opt, uint32_t out_clk)
{
    uint32_t SELR, SELI, SELP, in_clk;
    bool isFind = false;
    uint32_t M, m, N, n, P, Fcco, Fout;
    
    SYSCON->SYSPLLCLKSEL = SYSCON_SYSPLLCLKSEL_SEL(opt);
    
    if(opt == 0)
    {
        in_clk = 12*1000*1000;
    }

    if(opt == 1)
    {
     //   in_clk = GetClock(kExtOSCClock);
        SYSCON->AHBCLKCTRL[0] |= SYSCON_AHBCLKCTRL_IOCON_MASK;
        IOCON->PIO[0][22] &= ~IOCON_PIO_FUNC_MASK;
        IOCON->PIO[0][22] |= IOCON_PIO_FUNC(1) | IOCON_PIO_DIGIMODE_MASK;
    //    SetExtOSC(in_clk, true);
    }

    /* power off PLL */
    #if defined(SYSCON_PDRUNCFG_PDEN_VD3_MASK)
    SYSCON->PDRUNCFG[0] |= (SYSCON_PDRUNCFG_PDEN_SYS_PLL_MASK | SYSCON_PDRUNCFG_PDEN_VD3_MASK);
    #else
    SYSCON->PDRUNCFG[0] |= SYSCON_PDRUNCFG_PDEN_SYS_PLL_MASK;
    #endif
    
    #if defined(SYSCON_SYSPLLSSCTRL1_PD_MASK)
    SYSCON->SYSPLLSSCTRL1 |= SYSCON_SYSPLLSSCTRL1_PD_MASK;
    #endif
    
    /* n: pre divider, p: post div, m feedback */
    /* n:1-256, m:1-32768, p:1-32 */
    /* Fout = Fcco = 2 x M x Fin / N */
    for(m=1; m<=60; m++)
    {
        for(n=1; n<256; n++)
        {
            Fcco = 2 * m * in_clk / n;
            Fout = Fcco/2; /* P = 1 */

            if(Fout == out_clk && Fcco >= 60*1000*1000 && Fcco <= 500*1000*1000)
            {
                M = m;
                N = n;
                LIB_TRACE("m:%d, n:%d Fcco:%dHz\r\n", M, N, Fcco);
                isFind = true;
                goto FIND_SYS_PLL_PARAM;
            }
        }
    }

    if(isFind == false)
    {
        LIB_TRACE("not found!\r\n");
        return 1;
    }
    
    FIND_SYS_PLL_PARAM:
    
    SELR = 0U;
    SELI = (M & 0x3cU) + 4U;
    SELP = (M >> 1U) + 1U;
    
    #if defined(SYSCON_SYSPLLCTRL_BANDSEL_MASK)
    SYSCON->SYSPLLCTRL = SYSCON_SYSPLLCTRL_BANDSEL_MASK | SYSCON_SYSPLLCTRL_SELR(SELR) | SYSCON_SYSPLLCTRL_SELP(SELP) | SYSCON_SYSPLLCTRL_SELI(SELI) | SYSCON_SYSPLLCTRL_DIRECTO_MASK;
    #else
    SYSCON->SYSPLLCTRL =  SYSCON_SYSPLLCTRL_SELR(SELR) | SYSCON_SYSPLLCTRL_SELP(SELP) | SYSCON_SYSPLLCTRL_SELI(SELI);
    #endif
            //SYSCON_SYSPLLCTRL_DIRECTO_MASK
    
    #if defined(SYSCON_SYSPLLSSCTRL0_SEL_EXT_MASK)
    SYSCON->SYSPLLSSCTRL0 = SYSCON_SYSPLLSSCTRL0_SEL_EXT_MASK | pllEncodeM(M) | SYSCON_SYSPLLSSCTRL0_MREQ_MASK; /* M */
    #else
    SYSCON->SYSPLLMDEC = pllEncodeM(M) | SYSCON_SYSPLLMDEC_MREQ_MASK;
    #endif

    /* post div, div by 2 */
    P = 1;
    SYSCON->SYSPLLNDEC = SYSCON_SYSPLLNDEC_NREQ_MASK | pllEncodeN(N); /* N */
    SYSCON->SYSPLLPDEC = SYSCON_SYSPLLPDEC_PREQ_MASK | pllEncodeP(P); /* P */
        
    /* power up PLL */
    #if defined(SYSCON_PDRUNCFG_PDEN_VD3_MASK)
    SYSCON->PDRUNCFG[0] &= ~(SYSCON_PDRUNCFG_PDEN_SYS_PLL_MASK | SYSCON_PDRUNCFG_PDEN_VD3_MASK);
    #else
    SYSCON->PDRUNCFG[0] &= ~SYSCON_PDRUNCFG_PDEN_SYS_PLL_MASK;
    #endif

    CLOCK_SetFLASHAccessCyclesForFreq(Fout);
    
    /* wait lock */
    while((SYSCON->SYSPLLSTAT & SYSCON_SYSPLLSTAT_LOCK_MASK) == 0);
    
    /* select main clock to be PLL */
    SYSCON->MAINCLKSELB = SYSCON_MAINCLKSELB_SEL(2);
     
    SystemCoreClockUpdate();
    
    return 0;
}
/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t port_state = 0;
    uint32_t KEY_value = 0;

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
    POWER_SetVoltageForFreq(150*1000*1000);             /*!< Set voltage for the one of the fastest clock outputs: System clock output */
    CLOCK_SetFLASHAccessCyclesForFreq(150*1000*1000);   /*!< Set FLASH wait states for core */
    SetupSystemPLL(0, 75*1000*1000);
    SystemCoreClockUpdate();
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

		
    while (1)
    {		
				KEY_value = 0;
				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_1, 1);			
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1)& (1<<KEY_COLUMN_1));
				KEY_value= KEY_value << 0;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2)& (1<<KEY_COLUMN_2));
				KEY_value= KEY_value << 1;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3)& (1<<KEY_COLUMN_3));
				KEY_value= KEY_value << 2;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4)& (1<<KEY_COLUMN_4));
				KEY_value= KEY_value << 3;
				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_1, 0);			
			
				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_2, 1);			
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1)& (1<<KEY_COLUMN_1));
				KEY_value= KEY_value << 4;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2)& (1<<KEY_COLUMN_2));
				KEY_value= KEY_value << 5;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3)& (1<<KEY_COLUMN_3));
				KEY_value= KEY_value << 6;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4)& (1<<KEY_COLUMN_4));
				KEY_value= KEY_value << 7;
				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_2, 0);			

				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_3, 1);			
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1)& (1<<KEY_COLUMN_1));
				KEY_value= KEY_value << 8;\
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2)& (1<<KEY_COLUMN_2));
				KEY_value= KEY_value << 9;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3)& (1<<KEY_COLUMN_3));
				KEY_value= KEY_value <<10;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4)& (1<<KEY_COLUMN_4));
				KEY_value= KEY_value << 11;
				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_3, 0);			

				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_4, 1);			
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_1)& (1<<KEY_COLUMN_1));
				KEY_value= KEY_value << 12;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_2)& (1<<KEY_COLUMN_2));
				KEY_value= KEY_value << 13;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_3)& (1<<KEY_COLUMN_3));
				KEY_value= KEY_value << 14;
				KEY_value |= (GPIO_PinRead(GPIO, BOARD_KEY_PORT, KEY_COLUMN_4)& (1<<KEY_COLUMN_4));
				KEY_value= KEY_value << 15;
				GPIO_PinWrite(GPIO, BOARD_KEY_PORT, KEY_ROW_4, 0);			


			
			
//        port_state = GPIO_PortRead(GPIO, APP_SW_PORT);
//        if (!(port_state & (1 << APP_SW_PIN)))
//        {
//            PRINTF("\r\n Port state: %x\r\n", port_state);
//            GPIO_PortToggle(GPIO, APP_BOARD_TEST_LED_PORT, 1u << APP_BOARD_TEST_LED_PIN);
//        }
//        delay();
    }
}
