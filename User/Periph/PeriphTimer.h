#ifndef		__PERIPH_TIMER_H
#define		__PERIPH_TIMER_H

#ifdef		__cplusplus
 extern "C" {
#endif
	
	#include "stm32f4xx.h"
	
	void vTimer1Init(void);
	void vTimer3Init(void);
	void vTimer5Init(void);
	void vTimer2Init(void);
	void vTimer4Init(void);
	void vTimerRobotInit(void);
	int32_t xGetRobot2Cycle(void);
	int32_t xGetRobot1Cycle(void);
	
	#define vTimer3SetCycle(x)		TIM3->CCR1 = x // 电机速度调节
	 
	#define xTimer2GetCycle()		TIM2->CCR2 // 机械臂1的角度信号
	#define xTimer5GetCycle()		TIM5->CCR2 // 机械臂2的角度信号
	 
	#define xTimer4GetCnt()			TIM4->CNT // 冰箱门电机的编码器信号复位
	#define vTimer4CntReset()		TIM4->CNT = 0 // 冰箱门电机的编码器信号
	 
	#define xTimer1GetCnt()			TIM1->CNT // 取货电机的编码器信号
	#define vTimer1CntReset()		TIM1->CNT = 0 // 取货电机的编码器信号复位
	 
#ifdef		__cplusplus
 }
#endif

#endif
