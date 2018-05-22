#ifndef		__PERIPH_FSMC_H
#define		__PERIPH_FSMC_H

#ifdef		__cplusplus
 extern "C" {
#endif
	
	#include "stm32f4xx.h"

	void vFsmcInit(void);

	#define	FsmcBaseAddr			0x60000000

	#define FsmcWriteData(a, v)		*((volatile uint16_t *) (FsmcBaseAddr + a)) = v
	#define FsmcReadData(a)			*((volatile uint16_t *) (FsmcBaseAddr + a))

#ifdef		__cplusplus
 }
#endif

#endif
