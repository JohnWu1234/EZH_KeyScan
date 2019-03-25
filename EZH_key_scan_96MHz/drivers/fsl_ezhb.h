#ifndef _FSL_EZHB_H_
#define _FSL_EZHB_H_

//#define IMXRT500
#include <stddef.h>
#include "fsl_device_registers.h"
#include "fsl_common.h"


#if defined(IMXRT500)
//#define EZH_API_MEMORY_ADDRESS  0x00038000
#define EZH_API_MEMORY_ADDRESS 0x34100000
#else
#define EZH_API_MEMORY_ADDRESS  0x20010000
#endif


typedef struct {                                
  __IO uint32_t     EZHB_BOOT;            //0x20               
  __IO uint32_t     EZHB_CTRL;            //0x24               
  __I  uint32_t     EZHB_PC;              //0x28             
  __I  uint32_t     EZHB_SP;              //0x2C               
  __IO uint32_t     EZHB_BREAK_ADDR;      //0x30               
  __IO uint32_t     EZHB_BREAK_VECT;      //0x34          
  __IO uint32_t     EZHB_EMER_VECT;       //0x38          
  __IO uint32_t     EZHB_EMER_SEL;        //0x3C         
  __IO uint32_t     EZHB_ARM2EZH;         //0x40               
  __IO uint32_t     EZHB_EZH2ARM;         //0x44               
  __IO uint32_t     EZHB_PENDTRAP;  ///< Note : only available on versions 1.2 of EZH_dual_subsys or version 1.0.8 of EZH_arch_B_integration.v                        
 } EZH_ARCH_B_CON_Type;

 
#if defined(IMXRT500)
#define LPC_EZH_ARCH_B_CON_BASE    0x40027020 
#else
#define LPC_EZH_ARCH_B_CON_BASE    0x4001D020
#endif
#define LPC_EZH_ARCH_B0           ((volatile EZH_ARCH_B_CON_Type *)     LPC_EZH_ARCH_B_CON_BASE) 

#define EZHB_ARM2EZH_REGINT_MASK   (2)
#define EZHB_ARM2EZH_EVENTOUT_MASK (3)

void EZH_Init(void);
void EZH_Boot(void * pProgram, void *pPara, uint32_t mask);
void EZH_Deinit(void);
void EZH_Reset(void);

#endif

