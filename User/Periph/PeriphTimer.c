#include "Periph/PeriphTimer.h"

/**
 * @brief 518
 */
static int32_t Robot1Offset = 0;
static int32_t Robot2Offset = 0;

/** 
 * @brief 校准磁编码器的数据
 */
void vTimerRobotInit(void)
{
	int32_t v1 = xTimer2GetCycle();
	Robot1Offset = 518 - v1;
	
	v1 = xTimer5GetCycle();
	Robot2Offset = 518 - v1;
}

/**
 * @brief 获取机械臂1的磁编码器数据
 */
int32_t xGetRobot1Cycle(void)
{
	int32_t v = xTimer2GetCycle() + Robot1Offset;
	if (v < 0) {
		return v + 1036;
	}
	if (v > 1036) {
		return v - 1036;
	}
	return v;
}

/**
 * @brief 获取机械臂2的磁编码器数据
 */
int32_t xGetRobot2Cycle(void)
{
	int32_t v = xTimer5GetCycle() + Robot2Offset;
	if (v < 0) {
		return v + 1036;
	}
	if (v > 1036) {
		return v - 1036;
	}
	return v;
}

/**
 * @brief PC6 <-> TIM3_CH1 <-> AF2
 * 频率60KHz
 */
void vTimer3Init(void)
{
	RCC->AHB1ENR |= 0x04;
	GPIOC->MODER &= ~0x3000;
	GPIOC->MODER |= 0x2000;
	GPIOC->OSPEEDR |= 0x3000;
	GPIOC->PUPDR &= ~0x3000;
	GPIOC->OTYPER &= ~0x40;
	GPIOC->AFR[0] &= ~0x0F000000;
	GPIOC->AFR[0] |= 0x02000000;
	
	RCC->APB1ENR |= 0x02;
	TIM3->CR1 = 0x00;
	TIM3->CR2 = 0x00;
	TIM3->ARR = 100 - 1;
	TIM3->CCR1 = 0;
	TIM3->PSC = 84 * 2 - 1;
	TIM3->CCMR1 = 0x0070;
	TIM3->CCER = 0x0001;
	TIM3->EGR = 0x01;
	TIM3->CR1 = 0x01;
}

// Timer5 CH1 <-> PA0 <-> AF2
void vTimer5Init(void)
{
	RCC->AHB1ENR |= 0x01;
	GPIOA->MODER &= ~0x03;
	GPIOA->MODER |= 0x02;
	GPIOA->PUPDR &= ~0x03;
	GPIOA->OSPEEDR |= 0x03;
	GPIOA->OTYPER &= ~0x01;
	GPIOA->AFR[0] &= ~0x0F;
	GPIOA->AFR[0] |= 0x02;
	
	RCC->APB1ENR |= 0x08;
	TIM5->CR1 = 0x00;
	TIM5->CR2 = 0x00;
	TIM5->PSC = 84 - 1;
	TIM5->ARR = 0xFFFFFFFF;
	TIM5->CCMR1 = 0x0201;
	TIM5->CCER = 0x31;
	TIM5->SMCR = 0x54;
	TIM5->CR1 = 0x01;
	TIM5->EGR = 0x01;
}

// Timer2 CH1 <-> PA15 <-> AF1
void vTimer2Init(void)
{
	RCC->AHB1ENR |= 0x01;
	GPIOA->MODER &= ~0xC0000000;
	GPIOA->MODER |= 0x80000000;
	GPIOA->PUPDR &= ~0xC0000000;
	GPIOA->OSPEEDR |- 0xC0000000;
	GPIOA->OTYPER &= ~0x8000;
	GPIOA->AFR[1] &= ~0xF0000000;
	GPIOA->AFR[1] |= 0x10000000;
	
	RCC->APB1ENR |= 0x01;
	TIM2->CR1 = 0x00;
	TIM2->CR2 = 0x00;
	TIM2->PSC = 84 - 1;
	TIM2->ARR = 0xFFFFFFFF;
	TIM2->CCMR1 = 0x0201;
	TIM2->CCER = 0x0031;
	TIM2->SMCR = 0x54;
	TIM2->CR1 = 0x01;
	TIM2->EGR = 0x01;
}

/**
 * @brief TIM4_CH1 为冰箱门电机编码器计数 <-> PD12 <-> AF2
 */
void vTimer4Init(void)
{
	RCC->AHB1ENR |= 0x08;
	RCC->APB1ENR |= 0x04; // Timer4
	
	GPIOD->MODER &= ~0x3000000;
	GPIOD->MODER |= 0x2000000;
	
	GPIOD->PUPDR &= ~0x3000000;
	GPIOD->PUPDR |= 0x1000000;
	
	GPIOD->OSPEEDR |= 0x3000000;
	
	GPIOD->OTYPER &= ~0x1000;
	
	GPIOD->AFR[1] &= ~0xF0000;
	GPIOD->AFR[1] |= 0x20000;
	
	TIM4->CR1 = 0x00;
	TIM4->CR2 = 0x00;
	TIM4->SMCR = 0x57;
	TIM4->CCMR1 = 0x01;
	TIM4->CCER &= ~0x0A;
	TIM4->CR1 = 0x01;
}

/**
 * @brief TIM1_CH1 取货电机编码器计数 PE9 <-> TIM1_CH1 <-> AF1
 */ 
void vTimer1Init(void)
{
	RCC->AHB1ENR |= 0x10; // Enable GPIOE
	RCC->APB2ENR |= 0x01; // Enable TIM1
	
	GPIOE->MODER &= ~0xC0000;
	GPIOE->MODER |= 0x80000;
	
	GPIOE->PUPDR &= ~0xC0000;
	GPIOE->PUPDR |= 0x40000;
	
	GPIOE->OSPEEDR |= 0xC0000;
	
	GPIOE->OTYPER &= ~0x200;
	
	GPIOE->AFR[1] &= ~0xF0;
	GPIOE->AFR[1] |= 0x10;
	
	TIM1->CR1 = 0x00;
	TIM1->CR2 - 0x00;
	TIM1->SMCR = 0x57;
	TIM1->CCMR1 = 0x01;
	TIM1->CCER &= ~0x0A;
	TIM1->CR1 = 0x01;
}



