/*
 * @brief IOH Architecture B mnemonics
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "fsl_ezhb.h"
#include "fsl_ezhb_isr.h"
#include "fsl_debug_console.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* LPC51U68 */
#define EZH_BREAKADDR              0x1D030
#define EZH_BREAKVECT              0x1D034
#define EZH_EMERVECT               0x1D038
#define EZH_EMERSEL                0x1D03C
#define EZH_ARM2EZH                0x1D040
#define EZH_EZH2ARM                0x1D044 

#define EZH_CODE	__attribute__((section("EZH_SECT"))) 

#define KEY_ROW_1      5
#define KEY_ROW_2      18
#define KEY_ROW_3      7
#define KEY_ROW_4      21

#define KEY_COLUMN_1   8
#define KEY_COLUMN_2   9
#define KEY_COLUMN_3   10
#define KEY_COLUMN_4   31


#define PINFUNC_EZH		12
#define PINFUNC_GPIO	0
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Active key scan when key row and colume > 8 */

__asm void EZH_CODE EZH_KeyScan(void)
{
    E_NOP
    E_PER_READ(R6, EZH_ARM2EZH)
    E_LSR(R6, R6, 2)
    E_LSL(R6, R6, 2)
    E_LDR(SP, R6, 0)

    /* Set all row as low */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_1)
    E_BCLR_IMM(GPO, GPO, KEY_ROW_2)
    E_BCLR_IMM(GPO, GPO, KEY_ROW_3)
    E_BCLR_IMM(GPO, GPO, KEY_ROW_4)

    /* Set all row as output */
    E_BSET_IMM(GPD, GPD, KEY_ROW_1)
    E_BSET_IMM(GPD, GPD, KEY_ROW_2)
    E_BSET_IMM(GPD, GPD, KEY_ROW_3)
    E_BSET_IMM(GPD, GPD, KEY_ROW_4)

    /* Set all column as input */
    E_BCLR_IMM(GPD, GPD, KEY_COLUMN_1)
    E_BCLR_IMM(GPD, GPD, KEY_COLUMN_2)
    E_BCLR_IMM(GPD, GPD, KEY_COLUMN_3)
    E_BCLR_IMM(GPD, GPD, KEY_COLUMN_4)
		
    
kdown /* detect key down */		
    E_LOAD_IMM(R0, 0)
    E_LOAD_IMM(R1, 1)
    /* Scan 1st row */
    E_BSET_IMM(GPO, GPO, KEY_ROW_1)
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_1)
    E_COND_OR(NZ, R0, R0, R1) /* bit 0 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 1) /* bit 1 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_3)
    E_COND_LSL_OR(NZ, R0, R0, R1, 2) /* bit 2 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_4)
    E_COND_LSL_OR(NZ, R0, R0, R1, 3) /* bit 3 */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_1)
		/* Scan 2nd row */		
    E_BSET_IMM(GPO, GPO, KEY_ROW_2)
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_1)
    E_COND_LSL_OR(NZ, R0, R0, R1, 4) /* bit 3 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 5) /* bit 1 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 6) /* bit 2 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_3)
    E_COND_LSL_OR(NZ, R0, R0, R1, 7) /* bit 3 */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_2)
		/* Scan 3rd row */		
    E_BSET_IMM(GPO, GPO, KEY_ROW_3)
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_1)
    E_COND_LSL_OR(NZ, R0, R0, R1, 8) /* bit 3 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 9) /* bit 1 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 10) /* bit 2 */	
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_3)
    E_COND_LSL_OR(NZ, R0, R0, R1, 11) /* bit 3 */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_3)	
		/* Scan 4th row */		
    E_BSET_IMM(GPO, GPO, KEY_ROW_4)		
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_1)
    E_COND_LSL_OR(NZ, R0, R0, R1, 12) /* bit 3 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 13) /* bit 1 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 14) /* bit 2 */	
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_3)
    E_COND_LSL_OR(NZ, R0, R0, R1, 15) /* bit 3 */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_4)

		E_MOVS(R0, R0)
		E_COND_GOTO(NZ, kup)	/* if key down, go to detect key up */	
    E_GOTO(kdown)		
		
kup  /* detect key up */	
    E_LOAD_IMM(R0, 0)
    E_LOAD_IMM(R1, 1)
    /* Scan 1st row */
    E_BSET_IMM(GPO, GPO, KEY_ROW_1)
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_1)
    E_COND_OR(NZ, R0, R0, R1) /* bit 0 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 1) /* bit 1 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 2) /* bit 2 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_3)
    E_COND_LSL_OR(NZ, R0, R0, R1, 3) /* bit 3 */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_1)
		/* Scan 2nd row */		
    E_BSET_IMM(GPO, GPO, KEY_ROW_2)
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_1)
    E_COND_LSL_OR(NZ, R0, R0, R1, 4) /* bit 3 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 5) /* bit 1 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 6) /* bit 2 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_3)
    E_COND_LSL_OR(NZ, R0, R0, R1, 7) /* bit 3 */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_2)
		/* Scan 3rd row */		
    E_BSET_IMM(GPO, GPO, KEY_ROW_3)
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_1)
    E_COND_LSL_OR(NZ, R0, R0, R1, 8) /* bit 3 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 9) /* bit 1 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 10) /* bit 2 */	
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_3)
    E_COND_LSL_OR(NZ, R0, R0, R1, 11) /* bit 3 */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_3)	
		/* Scan 4th row */		
    E_BSET_IMM(GPO, GPO, KEY_ROW_4)		
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_1)
    E_COND_LSL_OR(NZ, R0, R0, R1, 12) /* bit 3 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 13) /* bit 1 */
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_2)
    E_COND_LSL_OR(NZ, R0, R0, R1, 14) /* bit 2 */	
    E_BTST_IMMS(GPI, GPI, KEY_COLUMN_3)
    E_COND_LSL_OR(NZ, R0, R0, R1, 15) /* bit 3 */
    E_BCLR_IMM(GPO, GPO, KEY_ROW_4)

		E_MOVS(R0, R0)
		E_COND_GOTO(ZE, to_int)	/* if key up, go to trigger ARM CORE */
		E_GOTO(kup)	/* if still key down, continue to detect key up */

to_int
		E_INT_TRIGGER(1234)	// notify app to update next foreground buffer address and fill count
		E_GOTO(kdown)		
}

void IOH_IRQHandler(){

		PRINTF("IOH enter");
//		switch(ezh_IO_read_R & 0xFFFF){
//			case 0x1:		PRINTF("key_value: S16\r\n");break;
//			case 0x2:		PRINTF("key_value: S15\r\n");break;
//			case 0x4:		PRINTF("key_value: S14\r\n");break;
//			case 0x8:		PRINTF("key_value: S13\r\n");break;
//			case 0x10:	PRINTF("key_value: S12\r\n");break;
//			case 0x20:	PRINTF("key_value: S11\r\n");break;
//			case 0x40:	PRINTF("key_value: S10\r\n");break;
//			case 0x80:	PRINTF("key_value: S9	\r\n");break;
//			case 0x100:	PRINTF("key_value: S8	\r\n");break;
//			case 0x200:	PRINTF("key_value: S7	\r\n");break;
//			case 0x400:	PRINTF("key_value: S6	\r\n");break;
//			case 0x800:	PRINTF("key_value: S5	\r\n");break;
//			case 0x1000:PRINTF("key_value: S4	\r\n");break;
//			case 0x2000:PRINTF("key_value: S3	\r\n");break;
//			case 0x4000:PRINTF("key_value: S2	\r\n");break;
//			case 0x8000:PRINTF("key_value: S1	\r\n");break;
//			default:	break;
//		}
}



void EZH_INT_Init(void){
		NVIC_EnableIRQ(IOH_IRQn);
}
void EZH_Pin_Init(void){
	// >>> configure pins
	IOCON->PIO[0][KEY_ROW_1] 			= PINFUNC_EZH | 1<<7 | 1<<8;
	IOCON->PIO[0][KEY_ROW_2]			= PINFUNC_EZH | 1<<7 | 1<<8;
	IOCON->PIO[0][KEY_ROW_3] 			= PINFUNC_EZH | 1<<7 | 1<<8;
	IOCON->PIO[0][KEY_ROW_4] 			= PINFUNC_EZH | 1<<7 | 1<<8;
	IOCON->PIO[0][KEY_COLUMN_1]		= PINFUNC_EZH | 1<<7 | 1<<8;
	IOCON->PIO[0][KEY_COLUMN_2] 	= PINFUNC_EZH | 1<<7 | 1<<8;
	IOCON->PIO[0][KEY_COLUMN_3] 	= PINFUNC_EZH | 1<<7 | 1<<8;
	IOCON->PIO[0][KEY_COLUMN_4]		= PINFUNC_EZH | 1<<7 | 1<<8;
}

