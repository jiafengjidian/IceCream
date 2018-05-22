#include "Device/DeviceCh451s.h"

#define ReadDout()		(GPIOE->IDR & 0x400)

#define HighClk()		GPIOE->BSRRL = 0x1000
#define LowClk()		GPIOE->BSRRH = 0x1000

#define HighLoad()		GPIOE->BSRRL = 0x100
#define LowLoad()		GPIOE->BSRRH = 0x100

#define HighDin()		GPIOE->BSRRL = 0x800
#define LowDin()		GPIOE->BSRRH = 0x800

void vCh451sDisplay(uint8_t bit)
{
	vCh451sWriteData(0x800 + (1 << bit));
	vCh451sWriteData(0x900 + (1 << bit));
	vCh451sWriteData(0xa00 + (1 << bit));
	vCh451sWriteData(0xb00 + (1 << bit));
	vCh451sWriteData(0xc00 + (1 << bit));
	vCh451sWriteData(0xd00 + (1 << bit));
	vCh451sWriteData(0xe00 + (1 << bit));
	vCh451sWriteData(0xf00 + (1 << bit));
}

static void delay(void)
{
	uint32_t i ;
	for (i = 0; i < 5; i ++);
}

void vCh451sWriteData(uint16_t command)
{
	uint8_t i ;
	for (i = 0; i < 12; i ++)
	{
		if (command & 0x01) {
			HighDin();
		} else {
			LowDin();
		}
		delay();
		HighClk();
		delay();
		LowClk();
		delay();
		command >>= 1;
	}
	LowLoad();
	delay();
	HighLoad();
	delay();
}

/**
 * @brief CH451S引脚分布
 * @DCLK PE12
 * @DIN PE11
 * @LOAD PE8
 * @DOUT PE10
 */
void vCh451sInit(void)
{
	RCC->AHB1ENR |= 0x10;
	
	GPIOE->MODER &= ~0x3F30000;
	GPIOE->MODER |= 0x1510000;
	GPIOE->PUPDR &= ~0x3F30000;
	GPIOE->PUPDR |= 0x100000;
	GPIOE->OTYPER &= ~0x1900;
	GPIOE->OSPEEDR |= 0x3C30000;
	
	HighDin();
	LowClk();
	HighLoad();
	vCh451sWriteData(0x201);
	vCh451sWriteData(0x401);
	vCh451sWriteData(0x580);
}


















