#include "Device/DeviceMoto.h"

void vMotoSetDir(MotoDir dir)
{
	GPIOE->BSRRL = 0x0C; // PE2, PE3
	
	switch (dir)
	{
		case CW: // ˳ʱ��
			GPIOE->BSRRH = 0x04;
			break;
		
		case CCW: // ��ʱ��
			GPIOE->BSRRH = 0x08;
			break;
		
		case PAUSE: // ֹͣ
			break;
	}
}

void vMotoSelect(MotoId id)
{
	//vMotoDisableAll();
	
	switch (id)
	{
		case ROBOT1: // ��е�۵��1
			GPIOE->BSRRH = 0x10;
			break;
		
		case ROBOT2: // ��е�۵��2
			GPIOE->BSRRH = 0x20;
			break;
		
		case REF: // �����ŵ��
			GPIOE->BSRRH = 0x40;
			break;
		
		case SHIPMENT: // ȡ�����
			GPIOE->BSRRH = 0x80;
			break;
		
		case AIR: // ��ѹ�����
			GPIOC->BSRRH = 0x200;
			break;
		
		case REV: // йѹ��
			GPIOD->BSRRH = 0x10;
			break;
	}
}

void vMotoDisableAll(void) // ֹͣ���е��
{
	GPIOE->BSRRL = 0xFC;
	GPIOC->BSRRL = 0x200;
	GPIOD->BSRRL = 0x10;
}

/**
 * @brief �����������
 * PE2, PE3 <-> ����������
 * PE4 <-> ��е�۵��1 �͵�ƽʹ��
 * PE5 <-> ��е�۵��2 �͵�ƽʹ��
 * PE6 <-> �����ŵ�� �͵�ƽʹ��
 * PE7 <-> ȡ����� �͵�ƽʹ��
 * PC9 <-> ��ѹ����� �͵�ƽʹ��
 * PD4 <-> йѹ�� �͵�ƽʹ��
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














