#ifndef		__DEVICE_NRF2401_H
#define		__DEVICE_NRF2401_H

#ifdef		__cplusplus
 extern "C" {
#endif

	#include "stm32f4xx.h"
	
	uint8_t xNrfCheck(void);

	void vNrf2401Init(void);
	 
	void vNrfEnterTxMode(void);
	 
	uint8_t xNrfSendPacket(const uint8_t *packet, uint8_t len);
	 
	#define MAX_TX  	    0x10    //�ﵽ����ʹ����ж�
	#define TX_OK       	0x20    //TX��������ж�
	#define RX_OK   	    0x40    //���յ������ж�

#ifdef		__cplusplus
 }
#endif

#endif
