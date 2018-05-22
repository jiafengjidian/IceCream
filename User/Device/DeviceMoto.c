#include "Device/DeviceMoto.h"

void vMotoSetDir(MotoDir dir)
{
	GPIOE->BSRRL = 0x0C; // PE2, PE3
	
	switch (dir)
	{
		case CW: // 顺时针
			GPIOE->BSRRH = 0x04;
			break;
		
		case CCW: // 逆时针
			GPIOE->BSRRH = 0x08;
			break;
		
		case PAUSE: // 停止
			break;
	}
}

void vMotoSelect(MotoId id)
{
	//vMotoDisableAll();
	
	switch (id)
	{
		case ROBOT1: // 机械臂电机1
			GPIOE->BSRRH = 0x10;
			break;
		
		case ROBOT2: // 机械臂电机2
			GPIOE->BSRRH = 0x20;
			break;
		
		case REF: // 冰箱门电机
			GPIOE->BSRRH = 0x40;
			break;
		
		case SHIPMENT: // 取货电机
			GPIOE->BSRRH = 0x80;
			break;
		
		case AIR: // 空压机电机
			GPIOC->BSRRH = 0x200;
			break;
		
		case REV: // 泄压阀
			GPIOD->BSRRH = 0x10;
			break;
	}
}

void vMotoDisableAll(void) // 停止所有电机
{
	GPIOE->BSRRL = 0xFC;
	GPIOC->BSRRL = 0x200;
	GPIOD->BSRRL = 0x10;
}

/**
 * @brief 电机控制引脚
 * PE2, PE3 <-> 电机方向控制
 * PE4 <-> 机械臂电机1 低电平使能
 * PE5 <-> 机械臂电机2 低电平使能
 * PE6 <-> 冰箱门电机 低电平使能
 * PE7 <-> 取货电机 低电平使能
 * PC9 <-> 空压机电机 低电平使能
 * PD4 <-> 泄压阀 低电平使能
 */
void vMotoInit(void)
{
	RCC->AHB1ENR |= 0x10;
	GPIOE->MODER &= ~0xFFF0;
	GPIOE->MODER |= 0x5550;
	GPIOE->OTYPER &= ~0xFC;
	GPIOE->OSPEEDR |= 0xFFF0;
	GPIOE->PUPDR &= ~0xFFF0;
	GPIOE->BSRRL = 0xFC;
	
	RCC->AHB1ENR |= 0x04;
	GPIOC->MODER &= ~0xC0000;
	GPIOC->MODER |= 0x40000;
	GPIOC->OTYPER &= ~0x200;
	GPIOC->OSPEEDR |= 0xC0000;
	GPIOC->PUPDR &= ~0xC0000;
	GPIOC->BSRRL = 0x200;
	
	RCC->AHB1ENR |= 0x08;
	GPIOD->MODER &= ~0x300;
	GPIOD->MODER |= 0x100;
	GPIOD->OTYPER &= ~0x10;
	GPIOD->PUPDR &= ~0x300;
	GPIOD->OSPEEDR |= 0x300;
	GPIOD->BSRRL = 0x10;
}














