#include "Device/DeviceNrf2401.h"

const static uint8_t NrfTxAddress[] = {0xE5, 0xE5, 0xE5, 0xE5, 0xE5};
const static uint8_t NrfRxAddress[] = {0xE5, 0xE5, 0xE5, 0xE5, 0xE5};
const static uint8_t NrfRfChannel = 50;

static void vNrfGpioInit(void);
uint8_t xNrfReadRegister(uint8_t reg);
uint8_t vNrfSendPacket(const uint8_t *packet, uint8_t len);
uint8_t xNrfWriteRegister(uint8_t reg, uint8_t value);
uint8_t xNrfReadRegisterOfBuffer(uint8_t reg, uint8_t *buffer, uint8_t len);
uint8_t xNrfWriteRegisterOfBuffer(uint8_t reg, const uint8_t *buffer, uint8_t len);
static uint8_t xSpiTranslateByte(uint8_t byte);

#define CSLow()				GPIOD->BSRRH = 0x4000
#define CSHigh()			GPIOD->BSRRL = 0x4000

#define CELow()				GPIOD->BSRRH = 0x8000
#define CEHigh()			GPIOD->BSRRL = 0x8000

#define IrqRead()			(GPIOD->IDR & 0x200)

#define MisoRead()			(GPIOD->IDR & 0x400)

#define MosiLow()			GPIOD->BSRRH = 0x800
#define MosiHigh()			GPIOD->BSRRL = 0x800

#define SckLow()			GPIOD->BSRRH = 0x2000
#define SckHigh()			GPIOD->BSRRL = 0x2000

//NRF24L01寄存器操作命令
#define SPI_READ_REG    0x00  //读配置寄存器,低5位为寄存器地址
#define SPI_WRITE_REG   0x20  //写配置寄存器,低5位为寄存器地址
#define RD_RX_PLOAD     0x61  //读RX有效数据,1~32字节
#define WR_TX_PLOAD     0xA0  //写TX有效数据,1~32字节
#define FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NOP             0xFF  //空操作,可以用来读状态寄存器	 
//SPI(NRF24L01)寄存器地址
#define CONFIG          0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                              //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define EN_AA           0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define EN_RXADDR       0x02  //接收地址允许,bit0~5,对应通道0~5
#define SETUP_AW        0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define SETUP_RETR      0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define RF_CH           0x05  //RF通道,bit6:0,工作通道频率;
#define RF_SETUP        0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define STATUS          0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                              //bit5:数据发送完成中断;bit6:接收数据中断;
#define MAX_TX  	    0x10    //达到最大发送次数中断
#define TX_OK       	0x20    //TX发送完成中断
#define RX_OK   	    0x40    //接收到数据中断

#define OBSERVE_TX      0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define CD              0x09  //载波检测寄存器,bit0,载波检测;
#define RX_ADDR_P0      0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1      0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2      0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3      0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4      0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5      0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR         0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0        0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1        0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2        0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3        0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4        0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5        0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define FIFO_STATUS     0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                              //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;
#define DYNPD           0x1C  //使能pipe动态载荷长度 
#define FEATURE         0x1D  

void vNrf2401Init(void)
{
	vNrfGpioInit();
}

uint8_t xNrfSendPacket(const uint8_t *packet, uint8_t len)
{
	uint8_t status;
	CELow();
	xNrfWriteRegisterOfBuffer(WR_TX_PLOAD, packet, len);
	CEHigh();
	while (IrqRead() != 0); 
	status = xNrfReadRegister(STATUS);
	xNrfWriteRegister(SPI_WRITE_REG + STATUS, status);
	
	if (status & MAX_TX) {
		xNrfWriteRegister(FLUSH_TX, NOP);
		return MAX_TX;
	}
	
	if (status & TX_OK) {
		return TX_OK;
	}

	return status;
}

void vNrfEnterTxMode(void)
{
	CELow();
	
	/** 设置Tx结点地址 **/
	xNrfWriteRegisterOfBuffer(SPI_WRITE_REG + TX_ADDR, NrfTxAddress, sizeof(NrfTxAddress));
	xNrfWriteRegisterOfBuffer(SPI_WRITE_REG + RX_ADDR_P0, NrfRxAddress, sizeof(NrfRxAddress));
	
	// 使能通道0自动应答
	xNrfWriteRegister(SPI_WRITE_REG + EN_AA, 0x00);
	
	// 使能通道0的接收地址
	xNrfWriteRegister(SPI_WRITE_REG + EN_RXADDR, 0x01);
	
	// 设置自动重发时间间隔 500us + 86us 自动重发次数 10
	xNrfWriteRegister(SPI_WRITE_REG + SETUP_RETR, 0x21);
	
	// 设置RF通道
	xNrfWriteRegister(SPI_WRITE_REG + RF_CH, NrfRfChannel);
	
	// 使能通道0动态载荷长度
	xNrfWriteRegister(SPI_WRITE_REG + DYNPD, 0x01);
	
	// 使能动态载荷长度
	xNrfWriteRegister(SPI_WRITE_REG + FEATURE, 0x06);
	
	// 设置TX发射参数 0db 2Mbps 低噪声增益开启
	xNrfWriteRegister(SPI_WRITE_REG + RF_SETUP, 0x26);
	
	// 配置基本工作模式的参数 PWR_UP, EN_CRC, 16BIT_CRC. PRIM_RX 开启所有中断
	xNrfWriteRegister(SPI_WRITE_REG + CONFIG, 0x0E);
	
	CEHigh();
}

uint8_t xNrfCheck(void)
{
	uint8_t temp[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	uint8_t ret[sizeof(temp)];
	uint8_t i;
	xNrfWriteRegisterOfBuffer(SPI_WRITE_REG + TX_ADDR, temp, sizeof(temp));
	xNrfReadRegisterOfBuffer(TX_ADDR, ret, sizeof(temp));
	for (i = 0; i < sizeof(temp); i ++) {
		if (temp[i] != ret[i]) {
			return 1;
		}
	}
	return 0;
}

uint8_t xNrfWriteRegisterOfBuffer(uint8_t reg, const uint8_t *buffer, uint8_t len)
{
	uint8_t status, i;
	CSLow();
	status = xSpiTranslateByte(reg);
	for (i = 0; i < len; i ++) {
		xSpiTranslateByte(buffer[i]);
	}
	CSHigh();
	return status;
}

uint8_t xNrfReadRegisterOfBuffer(uint8_t reg, uint8_t *buffer, uint8_t len)
{
	uint8_t status, i;
	CSLow();
	status = xSpiTranslateByte(reg);
	for (i = 0; i < len; i ++) {
		buffer[i] = xSpiTranslateByte(NOP);
	}
	CSHigh();
	return status;
}

uint8_t xNrfReadRegister(uint8_t reg)
{
	uint8_t status;
	CSLow();
	xSpiTranslateByte(reg);
	status = xSpiTranslateByte(NOP);
	CSHigh();
	return status;
}

uint8_t xNrfWriteRegister(uint8_t reg, uint8_t value)
{
	uint8_t status;
	CSLow();
	status = xSpiTranslateByte(reg);
	xSpiTranslateByte(value);
	CSHigh();
	return status;
}

static uint8_t xSpiTranslateByte(uint8_t byte)
{
	uint8_t result = 0;
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		if (byte & 0x80) {
			MosiHigh();
		} else {
			MosiLow();
		}
		SckHigh();
		byte <<= 1;
		result <<= 1;
		if (MisoRead()) {
			result ++;
		}
		SckLow();
	}
	return result;
}

/**
 * @brief NRF2401L驱动程序
 * PD9 <-> IRQ // 浮空输入
 * PD10 <-> MISO // 浮空输入
 * PD11 <-> MOSI // 推挽输出
 * PD13 <-> SCK // 推挽输出
 * PD14 <-> CS // 推挽输出
 * PD15 <-> CE // 推挽输出
 */
static void vNrfGpioInit(void)
{
	RCC->AHB1ENR |= 0x08;
	GPIOD->MODER &= ~0xFCFC0000;
	GPIOD->MODER |= 0x54400000;
	GPIOD->OTYPER &= ~0xE800;
	GPIOD->PUPDR &= ~0xFCFC0000;
	GPIOD->OSPEEDR |= 0xFCFC0000;
	SckLow();
	CELow();
	CSHigh();
}
















