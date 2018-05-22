#include "Device/Device74HC165.h"

#define vPLLow()		GPIOE->BSRRH = 0x8000
#define vPLHigh()		GPIOE->BSRRL = 0x8000

#define vSckLow()		GPIOE->BSRRH = 0x2000
#define vSckHigh()		GPIOE->BSRRL = 0x2000

#define xQRead()		(GPIOE->IDR & 0x4000)

/**
 * @brief 	PE15 <-> PL
 * 			PE13 <-> SCK
 * 			PE14 <-> Q7
 */
void v74HC165Init(void)
{
	RCC->AHB1ENR |= 0x10;
	
	GPIOE->MODER &= ~0xFC000000;
	GPIOE->MODER |= 0x44000000;
	GPIOE->PUPDR &= 0xFC000000;
	GPIOE->PUPDR |= 0x10000000;
	GPIOE->OSPEEDR |= 0xFC000000;
	GPIOE->OTYPER &= ~0xA000;
	vPLHigh();
	vSckLow();
}	

uint8_t x74HC165ReadByte(void)
{
	uint8_t byte = 0;
	uint8_t i;
	vPLLow();
	vPLHigh();
	for (i = 0; i < 8; i ++)
	{
		vSckHigh();
		byte <<= 1;
		if (xQRead()) {
			byte ++;
		}
		vSckLow();
	}
	return byte;
}



