#ifndef		__SYS_TIMER_H
#define		__SYS_TIMER_H

#ifdef		__cplusplus
 extern "C" {
#endif
	
	#include "stm32f4xx.h"

	void vSysTimerDelay(uint32_t ms);
	
	void vSysTimerInit(void);

	#define COUNTOF(X)		(sizeof(X) / sizeof(X[0]))

#ifdef		__cplusplus
 }
#endif

#endif
