#include "Device/DeviceBluetooth.h"
#include "Task/BluetoothReceiverTask.h"

#define BLE_RECV_BUFF_SIZE			1024
#define BLE_RECV_MAX_CMD_SIZE		20

static void vBluetoothGpioInit(void);
static void vBluetoothReceiverDmaInit(void);
static void vBluetoothBufferOver(void);

static uint8_t BluetoothReceiverBuffer[BLE_RECV_BUFF_SIZE]; // 蓝牙接收缓冲区
static uint8_t BluetoothBuffer[BLE_RECV_MAX_CMD_SIZE]; // 蓝牙接收备份缓冲区 单条命令的最大长度
static uint32_t RecvStartIndex = 0; // 缓冲区索引
static uint32_t RecvEndIndex = 0; // 缓冲区索引

static BluetoothReceiverType BluetoothRecver; // 蓝牙BLE数据接收者 保存当前接收到的数据

static osMutexId_t BluetoothMutex; // Bluetooth的互斥锁
static osMutexAttr_t BluetoothMutextAttr = {
	"BluetoothMutext",
	osMutexRecursive | osMutexPrioInherit,
	NULL,
	NULL,
};

/**
 * @brief 蓝牙BLE初始化
 */
void vBluetoothInit(void)
{
	RCC->APB1ENR |= 0x20000;
	
	vBluetoothGpioInit();
	
	USART2->CR1 = 0x00;
	USART2->CR2 = 0x00;
	USART2->CR3 = 0x40;
	USART2->BRR = 42000000 / 115200;
	USART2->CR1 = 0x201C;
	
	vBluetoothReceiverDmaInit();
	NVIC_EnableIRQ(USART2_IRQn);
	
	BluetoothMutex = osMutexNew(&BluetoothMutextAttr); // 创建蓝牙的互斥锁
}

/**
 * @brief 使用蓝牙BLE发送字符串
 */
void vBluetoothWriteString(const char *string) 
{
	osMutexAcquire(BluetoothMutex, osWaitForever);
	while (*string != '\0') {
		vBluetoothWriteByte((uint8_t) *string);
		string ++;
	}
	osMutexRelease(BluetoothMutex);
}

/**
 * @brief 使用蓝牙BLE发送缓冲区中的数据
 */
void vBluetoothWriteByteArray(const uint8_t *byteArray, uint16_t length)
{
	osMutexAcquire(BluetoothMutex, osWaitForever);
	osDelay(100);
	while (length --) {
		vBluetoothWriteByte(*byteArray ++);
	}
	osMutexRelease(BluetoothMutex);
}

/**
 * @brief 使用蓝牙BLE发送缓冲区中的数据
 */
void vBluetoothWrite(const BluetoothReceiverType *type)
{
	uint32_t len = type->length;
	const uint8_t *byteArray = type->byteArray;
	osMutexAcquire(BluetoothMutex, osWaitForever);
	while (len --) {
		vBluetoothWriteByte(*byteArray ++);
	}
	osMutexRelease(BluetoothMutex);
}

/**
 * @brief 蓝牙BLE发送一个字节的数据
 */
void vBluetoothWriteByte(uint8_t byte)
{
	while (0 == (USART2->SR & 0x80));
	USART2->DR = byte;
}

/**
 * @brief 串口2中断服务函数
 */
void USART2_IRQHandler(void)
{
	if (0x00 == (USART2->SR & 0x10)) {
		return;
	}
	
	RecvEndIndex = USART2->DR; // 请空接收标志位
	RecvEndIndex = sizeof(BluetoothReceiverBuffer) - DMA1_Stream5->NDTR; 
	
	if (RecvEndIndex < RecvStartIndex) {
		vBluetoothBufferOver();
	} else {
		BluetoothRecver.byteArray = BluetoothReceiverBuffer + RecvStartIndex;
		BluetoothRecver.length = RecvEndIndex - RecvStartIndex;
	}
	
	RecvStartIndex = RecvEndIndex;
	
	osMessageQueuePut(BluetoothReceiverMQId, &BluetoothRecver, 0, 0);
}

/**
 * @brief 缓冲区溢出
 */
static void vBluetoothBufferOver(void)
{
	uint16_t i;
	uint16_t offset = sizeof(BluetoothReceiverBuffer) - RecvStartIndex;
	for (i = RecvStartIndex; i < sizeof(BluetoothReceiverBuffer); i ++) {
		BluetoothBuffer[i - RecvStartIndex] = BluetoothReceiverBuffer[i];
	}
	for (i = 0; i < RecvEndIndex; i ++) {
		BluetoothBuffer[offset + i] = BluetoothReceiverBuffer[i];
	}
	BluetoothRecver.byteArray = BluetoothBuffer;
	BluetoothRecver.length = offset + RecvEndIndex;
}

/**
 * @brief DMA1_Stream5 Channel4
 */
static void vBluetoothReceiverDmaInit(void)
{
	RCC->AHB1ENR |= 0x200000;
	
	DMA1->HIFCR = 0xFC0;
	DMA1_Stream5->CR = 0x00;
	DMA1_Stream5->M0AR = (uint32_t) BluetoothReceiverBuffer;
	DMA1_Stream5->PAR = (uint32_t) &USART2->DR;
	DMA1_Stream5->NDTR = sizeof(BluetoothReceiverBuffer);
	DMA1_Stream5->CR = 0x8000501;
}

/**
 * @brief 蓝牙BLE串口初始化
 * EN <-> PD7 推挽输出
 * TX <-> PD6 复用
 * RX <-> PD5 复用
 * STATE <-> PD8 浮空输入
 */
static void vBluetoothGpioInit(void)
{
	RCC->AHB1ENR |= 0x08;
	
	GPIOD->MODER &= ~0x3FC00;
	GPIOD->MODER |= 0x06800;
	GPIOD->PUPDR &= ~0x3FC00;
	GPIOD->PUPDR |= 0x1400;
	GPIOD->OSPEEDR |= 0xFC00;
	GPIOD->OTYPER &= ~0xE0;
	GPIOD->AFR[0] &= ~0xFF00000;
	GPIOD->AFR[0] |= 0x7700000;
	GPIOD->BSRRH = 0x80;
}








