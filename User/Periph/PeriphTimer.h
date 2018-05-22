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
	
	#define vTimer3SetCycle(x)		TIM3->CCR1 = x // ����ٶȵ���
	 
	#define xTimer2GetCycle()		TIM2->CCR2 // ��е��1�ĽǶ��ź�
	#define xTimer5GetCycle()		TIM5->CCR2 // ��е��2�ĽǶ��ź�
	 
	#define xTimer4GetCnt()			TIM4->CNT // �����ŵ���ı������źŸ�λ
	#define vTimer4CntReset()		TIM4->CNT = 0 // �����ŵ���ı������ź�
	 
	#define xTimer1GetCnt()			TIM1->CNT // ȡ������ı������ź�
	#define vTimer1CntReset()		TIM1->CNT = 0 // ȡ������ı������źŸ�λ
	 
#ifdef		__cplusplus
 }
#endif

#endif
