#ifndef		__74HC165_H
#define		__74HC165_H

#ifdef		__cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
	 
	void v74HC165Init(void);
	
	uint8_t x74HC165ReadByte(void);

	 
#ifdef		__cplusplus
 }
#endif

#endif
