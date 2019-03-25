/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_ezhb.h"
#include "ezh_key_scan.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
uint32_t ezh_stack[1];
EZH_KeyScan_queue queue_1; 
EZH_KeyScan_Para para;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);	
		CLOCK_EnableClock(kCLOCK_InputMux);
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
	
    BOARD_InitPins();
    BOARD_BootClockFROHF96M();
    BOARD_InitDebugConsole();

    PRINTF("EZH Key Scan demo.\r\n");
    PRINTF("Please touch any button.I can show your button number.\r\n");
	  EZH_Init();
		NVIC_EnableIRQ(IOH_IRQn);
	  EZH_Pin_Init();
    para.ezh_stack = (void *)ezh_stack;
	  para.pos = &queue_1.ezh_write;
	  para.p_buffer = (uint32_t *) queue_1.buf;
    EZH_Boot(EZH_KeyScan, &para, 0);
	
    while (1)
    {
        __WFI();
    }
		
}
