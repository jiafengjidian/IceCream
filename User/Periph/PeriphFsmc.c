#include "Periph/PeriphFsmc.h"

static void vFsmcGpioInit(void);


void vFsmcInit(void)
{
	vFsmcGpioInit();
	
	RCC->AHB3ENR |= 0x01;
	FSMC_Bank1->BTCR[0] = 0x5010;
	FSMC_Bank1->BTCR[1] = (2 << 8) + 5 + (0 << 28);
	FSMC_Bank1E->BWTR[0] = (2 << 8) + 5 + (0 << 28);
	FSMC_Bank1->BTCR[0] = 0x5011;
}

/**
 * @FsmcPin
 * D0 <-> PD14,  D1 <-> PD15,  D2 <-> PD0,   D3 <-> PD1
 * D4 <-> PE7,   D5 <-> PE8,   D6 <-> PE9,   D7 <-> PE10
 * D8 <-> PE11,  D9 <-> PE12,  D10 <-> PE13, D11 <-> PE14
 * D12 <-> PE15, D13 <-> PD8,  D14 <-> PD9,  D15 <-> PD10
 * A16 <-> PD11, NOE <-> PD4, NWE <-> PD5, NE1 <-> PD7
 * PD - 0, 1, 4, 5, 7, 8, 9, 10, 11, 14, 15
 * PE - 7, 8, 9, 10, 11, 12, 13, 14, 15
 **/
static void vFsmcGpioInit(void)
{
	RCC->AHB1ENR |= 0x08 + 0x10;
	
	GPIOD->MODER &= ~0xF0FFCF0F;
	GPIOD->MODER |= 0xA0AA8A0A;
	GPIOD->PUPDR &= ~0xF0FFCF0F;
	GPIOD->PUPDR |= 0x50554505;
	GPIOD->OSPEEDR |= 0xF0FFCF0F;
	GPIOD->OTYPER &= ~0xCFB3;
	GPIOD->AFR[0] &= ~0xF0FF00FF;
	GPIOD->AFR[0] |= 0xC0CC00CC;
	GPIOD->AFR[1] &= ~0xFF00FFFF;
	GPIOD->AFR[1] |= 0xCC00CCCC;
	
	GPIOE->MODER &= ~0xFFFFC000;
	GPIOE->MODER |= 0xAAAA8000;
	GPIOE->PUPDR &= ~0xFFFFC000;
	GPIOE->PUPDR |= 0x55554000;
	GPIOE->OSPEEDR |= 0xFFFFC000;
	GPIOE->OTYPER &= ~0xFF80;
	GPIOE->AFR[0] &= ~0xF0000000;
	GPIOE->AFR[0] |= 0xC0000000;
	GPIOE->AFR[1] = 0xCCCCCCCC;
}




