#ifndef		__DEVICE_CH451S_H
#define		__DEVICE_CH451S_H

#ifdef		__cplusplus
 extern "C" {
#endif

	#include "stm32f4xx.h" 
	
	void vCh451sInit(void);
	 
	void vCh451sWriteData(uint16_t command);
	 
	void vCh451sDisplay(uint8_t bit);


#ifdef		__cplusplus
 }
#endif

#endif
