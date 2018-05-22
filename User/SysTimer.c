#include "SysTimer.h"



void vSysTimerInit(void)
{
	SysTick->VAL = 0x00;
	SysTick->LOAD = 21000 - 1;
	SysTick->CTRL = 0x01;
}

void vSysTimerDelay(uint32_t ms)
{
	uint32_t oldValue = SysTick->VAL;
	uint32_t newValue;
	uint32_t tickValue = ms * 21000;
	uint32_t cntValue = 0;
	
	while (1)
	{
		newValue = SysTick->VAL;
		if (newValue != oldValue) 
		{
			if (newValue < oldValue) {
				cntValue += oldValue - newValue;
			} else {
				cntValue += oldValue + SysTick->LOAD - newValue;
			}
			if (cntValue >= tickValue) {
				return;
			}
			oldValue = newValue;
		}
	}
}








