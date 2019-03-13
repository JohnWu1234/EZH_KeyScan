#include "LPC51U68.h"
#include "fsl_ezhb_isr.h"
#include "fsl_ezhb.h"

#define PRI_EZHD(d)  (((d) & 3) << 12)
#define PRI_EZHI(d)  (((d) & 3) << 14)

#if defined(__ICCARM__)
#define EZH_IMAGE_START (uint32_t) __section_begin("__ezh_code_section")
#elif defined(__GNUC__)
extern const char ezh_image_start[];
extern int ezh_image_size;
#endif

void EZH_Init(void)
{
    CLOCK_EnableClock(kCLOCK_EzhArchB0);
#if defined(__ICCARM__)
    uint32_t ezh_image_size;
#pragma section = "__ezh_code_section"    
    ezh_image_size = (uint32_t)__section_end("__ezh_code_section") - (uint32_t)__section_begin("__ezh_code_section");
    memcpy((void *)EZH_API_MEMORY_ADDRESS, (void *)EZH_IMAGE_START, ezh_image_size);
#elif defined(__GNUC__)
    memcpy((void *)EZH_API_MEMORY_ADDRESS, (void *)ezh_image_start, ezh_image_size);
#endif
    LPC_EZH_ARCH_B0->EZHB_CTRL = (0xC0DE0000 | (1<<EZHB_ENABLE_GPISYNCH));
}

void EZH_Boot(void * pProgram, void *pPara, uint32_t mask) 
{
    uint32_t ezh_code_size = 0;
	
    LPC_EZH_ARCH_B0->EZHB_ARM2EZH = ((uint32_t)pPara | mask);
    LPC_EZH_ARCH_B0->EZHB_BOOT = (uint32_t) pProgram;
		SYSCON->AHBMATPRIO = PRI_EZHD(1) | PRI_EZHI(1);   //M6 - Inst, M7 - Data
//    SYSCTL0->AHBMATRIXPRIOR = PRI_EZHD(1) | PRI_EZHI(1);   //M6 - Inst, M7 - Data
    LPC_EZH_ARCH_B0->EZHB_CTRL = 0xC0DE0011 | (0<<EZHB_MASK_RESP) |(0<<EZHB_ENABLE_AHBBUF) ; // BOOT
};

void EZH_Deinit(void)
{
    LPC_EZH_ARCH_B0->EZHB_CTRL = 0xC0DE0000;
    CLOCK_DisableClock(kCLOCK_EzhArchB0);
}

void EZH_Reset(void)
{
    RESET_PeripheralReset(kEZH_ARCH_B0_RSTn);
    LPC_EZH_ARCH_B0->EZHB_CTRL = (0xC0DE0000 | (1<<EZHB_ENABLE_GPISYNCH));
}

