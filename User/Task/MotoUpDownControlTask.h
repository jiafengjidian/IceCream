#ifndef		__MOTO_UP_DOWN_CONTROL_TASK_H
#define		__MOTO_UP_DOWN_CONTROL_TASK_H

#ifdef		__cplusplus
 extern "C" {
#endif
	 
#include <stdint.h>

	 
void vMotoUpDownTaskStop(void);

void vMotoUpDownTaskStart(uint8_t arg);



#ifdef		__cplusplus
 }
#endif

#endif
