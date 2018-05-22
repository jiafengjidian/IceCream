#include "stm32f4xx.h"
#include "Periph/PeriphFsmc.h"
#include "Device/DeviceLcd.h"
#include "Device/DeviceCh451s.h"
#include "Device/DeviceBluetooth.h"
#include "Device/DeviceMoto.h"
#include "Device/DeviceNrf2401.h"
#include "Periph/PeriphTimer.h"
#include "Task/BluetoothReceiverTask.h"
#include "Task/Ch451sTask.h"
#include "Task/TimerTask.h"
#include "Task/MotoControlTask.h"
#include "usbd_usr.h"
#include "SysTimer.h"
#include "xString.h"
#include "Periph/PeriphGpio.h"
#include "Device/Device74HC165.h"
#include "Task/StatusCheckTask.h"
#include "rtx_os.h"

uint8_t temp[] = {'a', 'b', 'c', 'd', 'e'};
void vUart3WriteString(const char *str);
void vUart3Init(void);

static char buffer[100];

static void debug(void *arg)
{
	while (1)
	{
		xStrValueOfUInt(buffer, xTimer1GetCnt(), 10);
		vUart3WriteString(buffer);
		vUart3WriteString("\r\n");
		osDelay(500);
	}
}

int main(void)
{	
	vSysTimerInit();
	
	v74HC165Init();
	vNrf2401Init();
	vCh451sInit();
	vUart3Init();
	vTimer3Init();
	vTimer5Init();
	vTimer2Init();
	vTimer4Init();
	vTimer1Init();
	vBluetoothInit();
	vMotoInit();
	vMotoDisableAll();
	vSysTimerDelay(500);
	vMotoSelect(SHIPMENT);
	vSysTimerDelay(500);
	vMotoSetDir(CCW);
	vSysTimerDelay(500);
	vMotoSetSpeed(3);
	
	
	while (xNrfCheck());
	vNrfEnterTxMode();
	
	vTimer4CntReset();
	vTimer1CntReset();

//	osKernelInitialize();
//	
//	vBluetoothTaskStart();
//	vCh451sTaskStart();
//	osThreadNew(debug, NULL, NULL);
//	vStartStatusCheckTask();
//	
//	osKernelStart();

	while (1) 
	{
		
	}
}

void vUart3WriteByte(uint8_t byte)
{
	while (0 == (USART3->SR & 0x80));
	USART3->DR = byte;
}

void vUart3WriteString(const char *str)
{
	while (*str != '\0')
	{
		vUart3WriteByte((uint8_t) *str);
		str ++;
	}
}

void vUart3Init(void)
{
	RCC->AHB1ENR |= 0x02; // Enable GpioB
	RCC->APB1ENR |= 0x40000; // Enable USART3
	RCC->AHB1ENR |= 0x200000; // Enable DMA1
	
	GPIOB->MODER &= ~0xF00000;
	GPIOB->MODER |= 0xA00000;
	
	GPIOB->PUPDR &= ~0xF00000;
	GPIOB->PUPDR |= 0x500000;
	
	GPIOB->OSPEEDR |= 0xA00000;
	
	GPIOB->OTYPER &= ~0xC00;
	
	GPIOB->AFR[1] &= ~0xFF00;
	GPIOB->AFR[1] |= 0x7700;
	
	USART3->CR1 = 0x00;
	USART3->CR2 = 0x00;
	USART3->CR3 = 0x40;
	USART3->BRR = 42000000 / 115200;
	USART3->CR1 = 0x201C;
}



